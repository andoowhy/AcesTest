// Copyright 2021 Andrew Macdonald

#include "SGraphPinComponentStruct.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Editor.h"
#include "StructViewerModule.h"
#include "StructViewerFilter.h"
#include "ScopedTransaction.h"
#include "AssetRegistryModule.h"
#include "EdGraphSchema_K2.h"

#include "Component.h"

#define LOCTEXT_NAMESPACE "SGraphPinComponentStruct"

class UUserDefinedStruct;

class FComponentTypePinFilter : public IStructViewerFilter
{
public:
	virtual bool IsStructAllowed( const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<class FStructViewerFilterFuncs> InFilterFuncs ) override
	{
		const UScriptStruct* ComponentStruct = FComponent::StaticStruct();

		// If a child of the Component, but not itself
		const bool bBasedOnComponent = InStruct && InStruct->IsChildOf( ComponentStruct ) && ( InStruct != ComponentStruct );
		const bool bValidStruct = ( InStruct->GetOutermost() != GetTransientPackage() );

		return bBasedOnComponent && bValidStruct;
	}

	virtual bool IsUnloadedStructAllowed( const FStructViewerInitializationOptions& InInitOptions, const FName InStructPath, TSharedRef<class FStructViewerFilterFuncs> InFilterFuncs ) override
	{
		return false;
	}
};

void SGraphPinComponentStruct::Construct( const FArguments& InArgs, UEdGraphPin* InGraphPinObj )
{
	SGraphPin::Construct( SGraphPin::FArguments(), InGraphPinObj );
}

void SGraphPinComponentStruct::OnPickedNewComponentType( const UScriptStruct* chosenComponentType )
{
	if( GraphPinObj->IsPendingKill() )
	{
		return;
	}

	FString NewPath;
	if( chosenComponentType )
	{
		NewPath = chosenComponentType->GetPathName();
	}

	if( GraphPinObj->GetDefaultAsString() != NewPath )
	{
		const FScopedTransaction Transaction( NSLOCTEXT( "GraphEditor", "ChangeClassPinValue", "Change Class Pin Value" ) );
		GraphPinObj->Modify();

		AssetPickerAnchor->SetIsOpen( false );
		GraphPinObj->GetSchema()->TrySetDefaultObject( *GraphPinObj, const_cast<UScriptStruct*>(chosenComponentType) );
	}
}

FReply SGraphPinComponentStruct::OnClickUse()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	//if( GraphPinObj && GraphPinObj->GetSchema() )
	//{
	//	const UClass* PinRequiredParentClass = Cast<const UClass>( GraphPinObj->PinType.PinSubCategoryObject.Get() );
	//	ensure( PinRequiredParentClass );

	//	const UClass* SelectedClass = GEditor->GetFirstSelectedClass( PinRequiredParentClass );
	//	if( SelectedClass )
	//	{
	//		const FScopedTransaction Transaction( NSLOCTEXT( "GraphEditor", "ChangeClassPinValue", "Change Class Pin Value" ) );
	//		GraphPinObj->Modify();

	//		GraphPinObj->GetSchema()->TrySetDefaultObject( *GraphPinObj, const_cast<UClass*>( SelectedClass ) );
	//	}
	//}

	return FReply::Handled();
}

TSharedRef<SWidget> SGraphPinComponentStruct::GenerateAssetPicker()
{
	FStructViewerModule& StructViewerModule = FModuleManager::LoadModuleChecked<FStructViewerModule>( "StructViewer" );

	TSharedPtr<FComponentTypePinFilter> filter = MakeShareable( new FComponentTypePinFilter );

	FStructViewerInitializationOptions Options;
	Options.Mode = EStructViewerMode::StructPicker;
	Options.bShowNoneOption = true;
	Options.StructFilter = filter;

	return
		SNew( SBox )
		.WidthOverride( 280 )
		[
			SNew( SVerticalBox )

			+ SVerticalBox::Slot()
		.FillHeight( 1.0f )
		.MaxHeight( 500 )
		[
			SNew( SBorder )
				.Padding( 4 )
				.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
				[
					StructViewerModule.CreateStructViewer( Options, FOnStructPicked::CreateSP( this, &SGraphPinComponentStruct::OnPickedNewComponentType ) )
				]
			]
		];
}

FText SGraphPinComponentStruct::GetDefaultComboText() const
{
	return LOCTEXT( "DefaultComboText", "Select Component" );
}

FOnClicked SGraphPinComponentStruct::GetOnUseButtonDelegate()
{
	return FOnClicked::CreateSP( this, &SGraphPinComponentStruct::OnClickUse );
}

const FAssetData& SGraphPinComponentStruct::GetAssetData( bool bRuntimePath ) const
{
	if( bRuntimePath )
	{
		// For runtime use the default path
		return SGraphPinObject::GetAssetData( bRuntimePath );
	}

	FString CachedRuntimePath = CachedEditorAssetData.ObjectPath.ToString() + TEXT( "_C" );

	if( GraphPinObj->DefaultObject )
	{
		if( !GraphPinObj->DefaultObject->GetPathName().Equals( CachedRuntimePath, ESearchCase::CaseSensitive ) )
		{
			// This will cause it to use the UBlueprint
			CachedEditorAssetData = FAssetData( GraphPinObj->DefaultObject, false );
		}
	} else if( !GraphPinObj->DefaultValue.IsEmpty() )
	{
		if( !GraphPinObj->DefaultValue.Equals( CachedRuntimePath, ESearchCase::CaseSensitive ) )
		{
			FString EditorPath = GraphPinObj->DefaultValue;
			EditorPath.RemoveFromEnd( TEXT( "_C" ) );
			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

			CachedEditorAssetData = AssetRegistryModule.Get().GetAssetByObjectPath( FName( *EditorPath ) );

			if( !CachedEditorAssetData.IsValid() )
			{
				FString PackageName = FPackageName::ObjectPathToPackageName( EditorPath );
				FString PackagePath = FPackageName::GetLongPackagePath( PackageName );
				FString ObjectName = FPackageName::ObjectPathToObjectName( EditorPath );

				// Fake one
				CachedEditorAssetData = FAssetData( FName( *PackageName ), FName( *PackagePath ), FName( *ObjectName ), UObject::StaticClass()->GetFName() );
			}
		}
	} else
	{
		if( CachedEditorAssetData.IsValid() )
		{
			CachedEditorAssetData = FAssetData();
		}
	}

	return CachedEditorAssetData;
}

#undef LOCTEXT_NAMESPACE
