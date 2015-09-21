//
//  DeckController.h
//  MTG_iOS
//
//  Created by jeewoongpark on 9/20/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#ifndef MTG_iOS_DeckController_h
#define MTG_iOS_DeckController_h

#import <UIKit/UIKit.h>
#include "MTGController.h"

@interface DeckController : UIViewController {
@public NSMutableArray* deckViews;
@public NSMutableArray* deckImages;
@public NSMutableArray* stockViews;
@public NSMutableArray* stockImages;
@public NSMutableArray* currentLabels;
@public NSMutableArray* counts;
@public UIScrollView* allCardsView;
@public UIScrollView* deckView;
@public UILabel* statLabel;
@public int deck_index;
@public int category_index;
@public List* currentDeck;
@public int margin,width,height,labelWidth,labelHeight,buttonWidth,buttonHeight;
@public int creatureCount, spellCount, landCount, totalCount;
@public CGFloat coverWidth,coverHeight;
@public bool cacheImages;
    
    
}

- (void) showDeck;
- (void) showStock;
- (void) showCounts;
- (void) freeImages;
- (void) freeStockImages;

@end

#endif
