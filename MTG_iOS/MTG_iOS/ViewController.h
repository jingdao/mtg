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
    @public UIScrollView* opponentLands;
    @public UIScrollView* opponentBattlefield;
    @public UIScrollView* stackView;
    @public UIScrollView* selfBattlefield;
    @public UIScrollView* selfLands;
    @public UIScrollView* scrollView;
    @public UIImageView* popupImage;
    @public UIView* popupMask;
    @public UITextView* selfHP;
    @public UITextView* opponentHP;
    @public UIAlertView* mulliganAlert;
    @public MTGPlayer* player;
    @public char buffer[128];
}

@end

