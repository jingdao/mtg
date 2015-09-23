//
//  ViewController.h
//  MTG_iOS
//
//  Created by jeewoongpark on 9/7/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "MTGController.h"
#include "DeckController.h"

typedef enum {
    NONE,
    DISCARD,
    MANA,
    ATTACK,
    BLOCK,
    SELECT,
    SELECT_TARGET,
    STACK,
    WAIT,
    WAITATTACK
} Mode ;

@interface ViewController : UIViewController<UIActionSheetDelegate, UIPopoverControllerDelegate> {
    @public int width,height,margin,topmargin,numColumn,maxColumns;
    @public int popupWidth,popupHeight,textWidth,textHeight,buttonWidth,buttonHeight;
    @public int gridHeight,gridHeight2, labelWidth,labelHeight, stepperWidth, stepperHeight;
    @public CGFloat coverWidth,coverHeight,cardWidth,cardHeight,cardWidth2,cardHeight2;
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
    @public NSMutableArray* selfLabels;
    @public NSMutableArray* opponentLabels;
    @public NSMutableArray* messageQueue;

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
    @public UIAlertView* winnerAlert;
    @public UIView* manaAlert;
    @public UIButton* endturnButton;
    @public UIButton* attackButton;
    @public UIButton* confirmButton;
    @public UITextView* manaLabel;
    @public UIActionSheet* deckSheet;
    @public MTGPlayer* player;
    @public Permanent* currentPermanent;
    @public Permanent* currentEquipment;
    @public char buffer[128];
    @public int manaBuffer[6];
    @public Mode mode;
    @public void (*creature_callback)(Permanent*);
    @public int pendingMana;
    @public int block_index;
    @public int deck_index;
    @public List* attackerList;
    @public List* blockersList;
    @public List* opponentPermanents;
    @public bool canAttack;
    @public bool cacheImages;
}

@property (nonatomic, strong) DeckController* deckController;
@property (nonatomic,strong) UIPopoverController* deckPopover;

@end

