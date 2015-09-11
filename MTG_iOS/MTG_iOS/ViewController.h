//
//  ViewController.h
//  MTG_iOS
//
//  Created by jeewoongpark on 9/7/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "MTGController.h"

@interface ViewController : UIViewController<UIActionSheetDelegate> {
    @public int width,height,margin,topmargin,numColumn,maxColumns;
    @public int popupWidth,popupHeight,textWidth,textHeight,buttonWidth,buttonHeight;
    @public int gridHeight,gridHeight2;
    @public CGFloat cardWidth,cardHeight,cardWidth2,cardHeight2;
    @public UIImageView* selfDeck;
    @public UIImageView* opponentDeck;
    @public UIImage* coverImage;
    @public NSMutableArray* views;
    @public NSMutableArray* images;
    @public NSMutableArray* opponentLandsViews;
    @public NSMutableArray* opponentLandsImages;
    @public NSMutableArray* opponentBattlefieldViews;
    @public NSMutableArray* opponentBattlefieldImages;
    @public NSMutableArray* stackViews;
    @public NSMutableArray* stackImages;
    @public NSMutableArray* selfLandsViews;
    @public NSMutableArray* selfLandsImages;
    @public NSMutableArray* selfBattlefieldViews;
    @public NSMutableArray* selfBattlefieldImages;
    @public UIScrollView* opponentLands;
    @public UIScrollView* opponentBattlefield;
    @public UIScrollView* stack;
    @public UIScrollView* selfBattlefield;
    @public UIScrollView* selfLands;
    @public UIScrollView* scrollView;
    @public UIImageView* popupImage;
    @public UIView* popupMask;
    @public UITextView* selfHP;
    @public UITextView* opponentHP;
    @public UIAlertView* mulliganAlert;
    @public MTGPlayer* player;
    @public Permanent* currentPermanent;
    @public char buffer[128];
}

@end

