/*
 * (C) 2016 Sony Interactive Entertainment Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WKAXObject_h
#define WKAXObject_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    // must be same as WebCore::AXObjectCache::AXNotification
    kWKAXActiveDescendantChanged,
    kWKAXAutocorrectionOccured,
    kWKAXCheckedStateChanged,
    kWKAXChildrenChanged,
    kWKAXFocusedUIElementChanged,
    kWKAXLayoutComplete,
    kWKAXLoadComplete,
    kWKAXNewDocumentLoadComplete,
    kWKAXSelectedChildrenChanged,
    kWKAXSelectedTextChanged,
    kWKAXValueChanged,
    kWKAXScrolledToAnchor,
    kWKAXLiveRegionCreated,
    kWKAXLiveRegionChanged,
    kWKAXMenuListItemSelected,
    kWKAXMenuListValueChanged,
    kWKAXMenuClosed,
    kWKAXMenuOpened,
    kWKAXRowCountChanged,
    kWKAXRowCollapsed,
    kWKAXRowExpanded,
    kWKAXExpandedChanged,
    kWKAXInvalidStatusChanged,
    kWKAXTextChanged,
    kWKAXAriaAttributeChanged,
    kWKAXElementBusyChanged
};
typedef uint32_t WKAXNotification;

enum {
    // must be same as WebCore::AXObjectCache::AXTextChange
    kWKAXTextInserted,
    kWKAXTextDeleted,
};
typedef uint32_t WKAXTextChange;

enum {
    // must be same as WebCore::AXObjectCache::AXLoadingEvent
    kWKAXLoadingStarted,
    kWKAXLoadingReloaded,
    kWKAXLoadingFailed,
    kWKAXLoadingFinished,
};
typedef uint32_t WKAXLoadingEvent;

enum {
    // must be same as WebCore::AccessibilityRole
    kWKAnnotationRole = 1,
    kWKApplicationRole,
    kWKApplicationAlertRole,
    kWKApplicationAlertDialogRole,
    kWKApplicationDialogRole,
    kWKApplicationLogRole,
    kWKApplicationMarqueeRole,
    kWKApplicationStatusRole,
    kWKApplicationTimerRole,
    kWKAudioRole,
    kWKBlockquoteRole,
    kWKBrowserRole,
    kWKBusyIndicatorRole,
    kWKButtonRole,
    kWKCanvasRole,
    kWKCaptionRole,
    kWKCellRole,
    kWKCheckBoxRole,
    kWKColorWellRole,
    kWKColumnRole,
    kWKColumnHeaderRole,
    kWKComboBoxRole,
    kWKDefinitionRole,
    kWKDescriptionListRole,
    kWKDescriptionListTermRole,
    kWKDescriptionListDetailRole,
    kWKDetailsRole,
    kWKDirectoryRole,
    kWKDisclosureTriangleRole,
    kWKDivRole,
    kWKDocumentRole,
    kWKDocumentArticleRole,
    kWKDocumentMathRole,
    kWKDocumentNoteRole,
    kWKDocumentRegionRole,
    kWKDrawerRole,
    kWKEditableTextRole,
    kWKFooterRole,
    kWKFormRole,
    kWKGridRole,
    kWKGroupRole,
    kWKGrowAreaRole,
    kWKHeadingRole,
    kWKHelpTagRole,
    kWKHorizontalRuleRole,
    kWKIgnoredRole,
    kWKInlineRole,
    kWKImageRole,
    kWKImageMapRole,
    kWKImageMapLinkRole,
    kWKIncrementorRole,
    kWKLabelRole,
    kWKLandmarkApplicationRole,
    kWKLandmarkBannerRole,
    kWKLandmarkComplementaryRole,
    kWKLandmarkContentInfoRole,
    kWKLandmarkMainRole,
    kWKLandmarkNavigationRole,
    kWKLandmarkSearchRole,
    kWKLegendRole,
    kWKLinkRole,
    kWKListRole,
    kWKListBoxRole,
    kWKListBoxOptionRole,
    kWKListItemRole,
    kWKListMarkerRole,
    kWKMathElementRole,
    kWKMatteRole,
    kWKMenuRole,
    kWKMenuBarRole,
    kWKMenuButtonRole,
    kWKMenuItemRole,
    kWKMenuItemCheckboxRole,
    kWKMenuItemRadioRole,
    kWKMenuListPopupRole,
    kWKMenuListOptionRole,
    kWKOutlineRole,
    kWKParagraphRole,
    kWKPopUpButtonRole,
    kWKPreRole,
    kWKPresentationalRole,
    kWKProgressIndicatorRole,
    kWKRadioButtonRole,
    kWKRadioGroupRole,
    kWKRowHeaderRole,
    kWKRowRole,
    kWKRubyBaseRole,
    kWKRubyBlockRole,
    kWKRubyInlineRole,
    kWKRubyRunRole,
    kWKRubyTextRole,
    kWKRulerRole,
    kWKRulerMarkerRole,
    kWKScrollAreaRole,
    kWKScrollBarRole,
    kWKSearchFieldRole,
    kWKSheetRole,
    kWKSliderRole,
    kWKSliderThumbRole,
    kWKSpinButtonRole,
    kWKSpinButtonPartRole,
    kWKSplitGroupRole,
    kWKSplitterRole,
    kWKStaticTextRole,
    kWKSummaryRole,
    kWKSwitchRole,
    kWKSystemWideRole,
    kWKSVGRootRole,
    kWKTabGroupRole,
    kWKTabListRole,
    kWKTabPanelRole,
    kWKTabRole,
    kWKTableRole,
    kWKTableHeaderContainerRole,
    kWKTextAreaRole,
    kWKTreeRole,
    kWKTreeGridRole,
    kWKTreeItemRole,
    kWKTextFieldRole,
    kWKToggleButtonRole,
    kWKToolbarRole,
    kWKUnknownRole,
    kWKUserInterfaceTooltipRole,
    kWKValueIndicatorRole,
    kWKVideoRole,
    kWKWebAreaRole,
    kWKWebCoreLinkRole,
    kWKWindowRole,
};
typedef uint32_t WKAXRole;


enum {
    // must be same as WebCore::AccessibilityButtonState
    kWKButtonStateOff = 0,
    kWKButtonStateOn,
    kWKButtonStateMixed,
};
typedef uint32_t WKAXButtonState;

WK_EXPORT WKTypeID WKAXObjectGetTypeID();

WK_EXPORT WKPageRef WKAXObjectPage(WKAXObjectRef axObjectref);
WK_EXPORT WKFrameRef WKAXObjectFrame(WKAXObjectRef axObjectref);

WK_EXPORT WKAXRole WKAXObjectRole(WKAXObjectRef axObjectref);
WK_EXPORT WKStringRef WKAXObjectTitle(WKAXObjectRef axObject);
WK_EXPORT WKStringRef WKAXObjectDescription(WKAXObjectRef axObject);
WK_EXPORT WKStringRef WKAXObjectHelpText(WKAXObjectRef axObject);
WK_EXPORT WKStringRef WKAXObjectURL(WKAXObjectRef axObject);
WK_EXPORT WKAXButtonState WKAXObjectButtonState(WKAXObjectRef axObject);

#ifdef __cplusplus
}
#endif

#endif // WKAXObject_h
