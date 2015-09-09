//
//  ViewController.m
//  MTG_iOS
//
//  Created by jeewoongpark on 9/7/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import "ViewController.h"
#import <QuartzCore/QuartzCore.h>

@interface ViewController ()
- (void)updateHand: (List*)cards;
@end

CardData cd;
ViewController* viewController;

@implementation ViewController

- (void)updateHand: (List*)cards {
   
    [self->images removeAllObjects];
    for (UIImageView* iv in self->views) {
        [iv removeFromSuperview];
    }
    [self->views removeAllObjects];
    
    int numCards = cards->size > maxColumns ? maxColumns : cards->size;
    self->scrollView.contentSize = CGSizeMake((cardWidth+margin)*numCards,cardHeight);
    
    NSString* extension = @".jpg";
    for (unsigned int i=0;i<numCards && i<maxColumns;i++) {
        MTGCard* card = cards->entries[i];
        const char* tag = card->name;
        [self->views addObject:[[UIImageView alloc] initWithFrame:CGRectMake((margin+cardWidth)*i,0, cardWidth, cardHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        [self->images addObject:[UIImage imageNamed:fileName]];
        [self->views[i] setImage: self->images[i]];
        [self->scrollView addSubview:self->views[i]];
    }
    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    viewController = self;
    srand((unsigned int) time(NULL));
    
    self->coverImage = [UIImage imageNamed:@"cover.jpg"];
    self->width = self.view.frame.size.width;
    self->height = self.view.frame.size.height;
    self->margin = 5;
    self->numColumn = 6;
    self->maxColumns = 12;
    CGFloat coverWidth = self->coverImage.size.width;
    CGFloat coverHeight = self->coverImage.size.height;
    self->cardWidth = (width - (numColumn + 1)*margin)/numColumn;
    self->cardHeight = cardWidth/coverWidth*coverHeight;
    self->popupWidth = width/3;
    self->popupHeight = popupWidth/coverWidth*coverHeight;
    self->textWidth = 100;
    int divider = /*height-margin-cardHeight*/400;
    self->selfDeck = [[UIImageView alloc] initWithFrame:CGRectMake(width-margin*2-cardWidth-textWidth,divider-margin-cardHeight,cardWidth,cardHeight)];
    [self->selfDeck setImage: self->coverImage];
    self->selfDeck.userInteractionEnabled = YES;
    [self.view addSubview:self->selfDeck];
    self->opponentDeck = [[UIImageView alloc] initWithFrame:CGRectMake(margin,margin,cardWidth,cardHeight)];
    [self->opponentDeck setImage: self->coverImage];
    [self.view addSubview:self->opponentDeck];
    
    self->views = [[NSMutableArray alloc] init];
    self->images = [[NSMutableArray alloc] init];

     self->scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, divider, width-margin*2, cardHeight)];
    self->scrollView.contentSize = CGSizeMake((cardWidth+margin)*maxColumns,cardHeight);
    [self->scrollView setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->scrollView];
    
    self->popupMask = [[UIView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    self->popupMask.alpha = 0.5;
    [self->popupMask setBackgroundColor:[UIColor blackColor]];
    self->popupImage = [[UIImageView alloc] initWithFrame:CGRectMake((width-popupWidth)/2, (height-popupHeight)/2, popupWidth, popupHeight)];
    
    const CGFloat comp[3] = {0.5,0.5,0.5};
    CGColorRef col = CGColorCreate(CGColorSpaceCreateDeviceRGB(), comp);
    self->selfHP= [[UITextView alloc] initWithFrame:CGRectMake(width-margin-textWidth, divider-margin-cardHeight, textWidth, cardHeight)];
    [selfHP.layer setBorderColor:col];
    [selfHP.layer setBorderWidth:2.0];
    selfHP.layer.cornerRadius = 5;
    selfHP.clipsToBounds = YES;
    selfHP.editable = NO;
    [selfHP setFont: [UIFont boldSystemFontOfSize:25]];
    [self.view addSubview:selfHP];
    self->opponentHP= [[UITextView alloc] initWithFrame:CGRectMake(margin*2+cardWidth,margin, textWidth, cardHeight)];
    [opponentHP.layer setBorderColor:col];
    [opponentHP.layer setBorderWidth:2.0];
    opponentHP.layer.cornerRadius = 5;
    opponentHP.clipsToBounds = YES;
    opponentHP.editable = NO;
    [opponentHP setFont: [UIFont boldSystemFontOfSize:25]];
    [self.view addSubview:opponentHP];
    
    
    UITapGestureRecognizer *recog1 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog1.numberOfTapsRequired = 2;
    [self->scrollView addGestureRecognizer:recog1];
    
    UITapGestureRecognizer *recog2 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(exitTap:)];
    [self->popupMask addGestureRecognizer:recog2];
    
    UILongPressGestureRecognizer *recog3 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->scrollView addGestureRecognizer:recog3];
    
    UITapGestureRecognizer *recog4 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapDeck:)];
    [self->selfDeck addGestureRecognizer:recog4];
    
    cd = loadCardData();
    newGame();

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) doubleTap: (UITapGestureRecognizer*) gesture {
    CGPoint p = [gesture locationInView:self->scrollView];
    int idx = p.x / (self->cardWidth + self->margin);
    [self->popupImage setImage:self->images[idx]];
    [self.view addSubview:self->popupMask];
    [self.view addSubview:self->popupImage];
}
     
- (void) exitTap: (UITapGestureRecognizer*) gesture {
    [self->popupMask removeFromSuperview];
    [self->popupImage removeFromSuperview];
}

- (void) longpress: (UILongPressGestureRecognizer*) gesture {
    if (!(gesture.state==UIGestureRecognizerStateBegan))
        return;
    CGPoint p = [gesture locationInView:self->scrollView];
    int idx = p.x / (self->cardWidth + self->margin);
    if (MTGPlayer_playCard(self->player, idx, self->buffer)) {
        displayHand(self->player->hand);
        displayStats(self->player->hp,self->player->library->size, self->player->hand->size, true);
    } else {
        NSString *err = [NSString stringWithUTF8String:self->buffer];
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" message:err delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
    }
}

- (void) tapDeck: (UITapGestureRecognizer*) gesture {
    newTurn();
}

@end

void displayHand(List* cards) {
    [viewController updateHand:cards];
}

void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent) {
    NSString* hpString = [NSString stringWithFormat:@"HP: %d\nLibrary: %d\nHand: %d",hp,librarySize,handSize];
    if (selfOrOpponent) {
        [viewController->selfHP setText:hpString];
    } else {
        [viewController->opponentHP setText:hpString];
    }
}

void saveDeck(char* name,List* cards){
    
}

void loadDeck(char* name,List* cards) {
    
}

void startTurn(MTGPlayer* player) {
    viewController->player = player;
}
