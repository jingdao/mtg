//
//  DeckController.m
//  MTG_iOS
//
//  Created by jeewoongpark on 9/20/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "DeckController.h"

extern List* categories;

@interface DeckController ()

@end


@implementation DeckController

- (UIImage*) loadImage: (NSString*)name cached: (BOOL) isCached {
    if (isCached)
        return [UIImage imageNamed:name];
    else
        //return [UIImage imageWithContentsOfFile:name];
        return [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:name ofType:nil]];
}

- (void) showCounts {
    [statLabel setText:[NSString stringWithFormat:@"Creature: %d Spell: %d: Land: %d Total: %d",
                        creatureCount,spellCount,landCount,totalCount]];
}

- (void) showStock {
    List* stock = categories->entries[category_index];
    allCardsView.contentSize = CGSizeMake((coverWidth+margin)*stock->size,coverHeight);
    for (unsigned int i=0;i<stock->size;i++) {
        MTGCard* card = stock->entries[i];
        const char* tag = card->name;
        [stockViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake((margin+coverWidth)*i,margin, coverWidth, coverHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:@".jpg"];
        [stockImages addObject:[self loadImage:fileName cached:cacheImages]];
        [stockViews[i] setImage: stockImages[i]];
        [allCardsView addSubview:stockViews[i]];
    }
    
    [self.view addSubview:allCardsView];
}

- (void) showDeck{
    
    List* initDeck = InitList();
    currentDeck = InitList();
    buildDeck(initDeck, deck_index);
    qsort(initDeck->entries,initDeck->size,sizeof(MTGCard*),MTGCard_compare);
    
    [counts removeAllObjects];
    int n = 1;
    void* previous = initDeck->entries[0];
    for (unsigned int i = 1;i<initDeck->size;i++) {
        if (initDeck->entries[i] == previous) n++;
        else {
            AppendToList(currentDeck,previous);
            [counts addObject:[NSNumber numberWithInt:n]];
            previous = initDeck->entries[i];
            n = 1;
        }
    }
    AppendToList(currentDeck, previous);
    [counts addObject:[NSNumber numberWithInt:n]];
    
    int numCards = currentDeck->size;
    deckView.contentSize = CGSizeMake((coverWidth+margin)*numCards,coverHeight);
    NSString* extension = @".jpg";
    self->totalCount = initDeck->size;
    for (unsigned int i=0;i<numCards;i++) {
        MTGCard* card = currentDeck->entries[i];
        const char* tag = card->name;
        [deckViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake((margin+coverWidth)*i,margin, coverWidth, coverHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        [deckImages addObject:[self loadImage:fileName cached:cacheImages]];
        
        UIImageView *iv = deckViews[i];
        UILabel* lb = [[UILabel alloc] init];
        [currentLabels addObject:lb] ;
        NSNumber *n = counts[i];
        lb.text = [NSString stringWithFormat:@"%d",n.intValue];
        lb.textColor = [UIColor whiteColor];
        lb.backgroundColor = [UIColor blackColor];
        lb.textAlignment = NSTextAlignmentCenter;
        lb.frame = CGRectMake(0, iv.frame.size.height-labelHeight,labelWidth,labelHeight);
        lb.layer.cornerRadius = 10;
        lb.layer.masksToBounds = YES;
        
        if (card->subtypes.is_land) landCount+=n.intValue;
        else if (card->subtypes.is_creature) creatureCount+=n.intValue;
        else spellCount+=n.intValue;
        
        [iv addSubview:lb];
        [iv setImage: deckImages[i]];
        [deckView addSubview:deckViews[i]];
    }
    
    [self.view addSubview:deckView];
    DeleteList(initDeck);
}

- (void) freeImages {
    self->creatureCount = 0;
    self->spellCount = 0;
    self->landCount = 0;
    self->totalCount = 0;
    DeleteList(currentDeck);
    [deckImages removeAllObjects];
    for (UIImageView* iv in deckViews) {
        [iv removeFromSuperview];
    }
    [deckViews removeAllObjects];
    for (UILabel* lb in currentLabels) {
        [lb removeFromSuperview];
    }
    [currentLabels removeAllObjects];
}

- (void) freeStockImages {
    [stockImages removeAllObjects];
    for (UIImageView* iv in stockViews) {
        [iv removeFromSuperview];
    }
    [stockViews removeAllObjects];
}

- (void)viewDidLoad {
    self->width = self.view.frame.size.width;
    self->height = self.view.frame.size.height;
    self->margin = 5;
    self->labelWidth = coverWidth/3;
    self->labelHeight = labelWidth/2;
    self->category_index = 0;
    
    self->deckViews = [[NSMutableArray alloc] init];
    self->deckImages = [[NSMutableArray alloc] init];
    self->stockViews = [[NSMutableArray alloc] init];
    self->stockImages = [[NSMutableArray alloc] init];
    self->currentLabels = [[NSMutableArray alloc] init];
    self->counts = [[NSMutableArray alloc] init];
    
    self->allCardsView = [[UIScrollView alloc] initWithFrame:CGRectMake(0,margin,width-margin*2,coverHeight+margin*3)];
    //[self->allCardsView setShowsHorizontalScrollIndicator:NO];
    self->deckView = [[UIScrollView alloc] initWithFrame:CGRectMake(0,coverHeight+margin*3+buttonHeight,width-margin*2,coverHeight+margin*3)];
    //[self->deckView setShowsHorizontalScrollIndicator:NO];
    UITapGestureRecognizer *recog1 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog1.numberOfTapsRequired = 2;
    [self->allCardsView addGestureRecognizer:recog1];
    UITapGestureRecognizer *recog2 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog2.numberOfTapsRequired = 2;
    [self->deckView addGestureRecognizer:recog2];
    
    UIButton* saveButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [saveButton setFrame:CGRectMake(width-buttonWidth-margin, coverHeight+margin*3, buttonWidth, buttonHeight)];
    [saveButton setTitle:@"Save" forState:UIControlStateNormal];
    [saveButton addTarget:self action:@selector(onSave:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:saveButton];
    UIButton* nextButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [nextButton setFrame:CGRectMake(width-buttonWidth*2-margin, coverHeight+margin*3, buttonWidth, buttonHeight)];
    [nextButton setTitle:@"Next" forState:UIControlStateNormal];
    [nextButton addTarget:self action:@selector(onNext:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:nextButton];
    UIButton* previousButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [previousButton setFrame:CGRectMake(width-buttonWidth*3-margin*3, coverHeight+margin*3, buttonWidth, buttonHeight)];
    [previousButton setTitle:@"Previous" forState:UIControlStateNormal];
    [previousButton addTarget:self action:@selector(onPrevious:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:previousButton];
    
    self->statLabel = [[UILabel alloc] initWithFrame:CGRectMake(margin,coverHeight+margin*3,width-buttonWidth*3-margin*5,buttonHeight)];
    [self.view addSubview:statLabel];
    
    //[self showDeck];
    //[self showStock];
    //[self showCounts];
}

- (void) doubleTap: (UITapGestureRecognizer*) gesture {
    unsigned long idx;
    CGPoint p = [gesture locationInView:[gesture view]];
    idx = p.x / (self->coverWidth + self->margin);
    if ([gesture view] == self->allCardsView) {

    } else if ([gesture view] == self->deckView) {
        NSNumber *n = counts[idx];
        if (n.intValue > 1) {
            counts[idx] = [NSNumber numberWithInt:n.intValue-1];
            UILabel *lb = currentLabels[idx];
            n = counts[idx];
            [lb setText: [NSString stringWithFormat:@"%d",n.intValue]];
            
            MTGCard* card = currentDeck->entries[idx];
            if (card->subtypes.is_land) landCount--;
            else if (card->subtypes.is_creature) creatureCount--;
            else spellCount--;
            totalCount--;
            [self showCounts];
        }
    }
}

- (void) onNext: (id) sender {
    [self freeStockImages];
    category_index = category_index == 13 ? category_index = 0 : category_index+1;
    [self showStock];
}

- (void) onPrevious: (id) sender {
    [self freeStockImages];
    category_index = category_index == 0 ? category_index = 13 : category_index-1;
    [self showStock];
}

- (void) onSave: (id) sender {
    [self freeStockImages];
    [self freeImages];
    [self dismissViewControllerAnimated:YES completion:nil];
}

@end