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

@end

CardData cd;
ViewController* viewController;

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    viewController = self;
    srand((unsigned int) time(NULL));
    
    self->coverImage = [UIImage imageNamed:@"cover.jpg"];
    self->width = self.view.frame.size.width;
    self->height = self.view.frame.size.height;
    self->margin = 5;
    self->topmargin = 27;
    self->numColumn = 6;
    self->maxColumns = 12;
    CGFloat coverWidth = self->coverImage.size.width;
    CGFloat coverHeight = self->coverImage.size.height;
    self->gridHeight = (height-margin-topmargin)/16 * 3;
    self->cardHeight = self->gridHeight - margin*2;
    self->cardWidth = cardHeight/coverHeight*coverWidth;
    self->gridHeight2 = (height-margin-topmargin)/8;
    self->cardHeight2 = self->gridHeight2 - margin*2;
    self->cardWidth2 = cardHeight2/coverHeight*coverWidth;
    self->popupWidth = width/3;
    self->popupHeight = popupWidth/coverWidth*coverHeight;
    self->textWidth = 100;
    self->textHeight = 200;
    self->buttonWidth = 100;
    self->buttonHeight = 30;
    self->selfDeck = [[UIImageView alloc] initWithFrame:CGRectMake(width-margin*2-cardWidth-textWidth,height-margin-gridHeight*2-gridHeight2,cardWidth,cardHeight)];
    [self->selfDeck setImage: self->coverImage];
    self->selfDeck.userInteractionEnabled = YES;
    [self.view addSubview:self->selfDeck];
    self->opponentDeck = [[UIImageView alloc] initWithFrame:CGRectMake(margin,topmargin,cardWidth,cardHeight)];
    [self->opponentDeck setImage: self->coverImage];
    [self.view addSubview:self->opponentDeck];
    
    self->opponentBattlefieldViews = [[NSMutableArray alloc] init];
    self->opponentBattlefieldImages = [[NSMutableArray alloc] init];
    self->opponentLandsViews = [[NSMutableArray alloc] init];
    self->opponentLandsImages = [[NSMutableArray alloc] init];
    self->stackViews = [[NSMutableArray alloc] init];
    self->stackImages = [[NSMutableArray alloc] init];
    self->selfBattlefieldViews = [[NSMutableArray alloc] init];
    self->selfBattlefieldImages = [[NSMutableArray alloc] init];
    self->selfLandsViews = [[NSMutableArray alloc] init];
    self->selfLandsImages = [[NSMutableArray alloc] init];
    self->views = [[NSMutableArray alloc] init];
    self->images = [[NSMutableArray alloc] init];

    self->opponentLands = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin, width-margin*5-cardWidth-textWidth-buttonWidth, gridHeight2)];
    self->opponentLands.contentSize = CGSizeMake((cardHeight2+margin)*maxColumns,gridHeight2);
    [self->opponentLands setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->opponentLands];
    [self->opponentLands setBackgroundColor:[UIColor redColor]];
    
    self->opponentBattlefield = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin+gridHeight2, width-margin*5-cardWidth-textWidth-buttonWidth, gridHeight)];
    self->opponentBattlefield.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->opponentBattlefield setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->opponentBattlefield];
    [self->opponentBattlefield setBackgroundColor:[UIColor greenColor]];
    
    self->stack = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin+gridHeight+gridHeight2, width-margin*6-cardWidth*2-textWidth*2, gridHeight)];
    self->stack.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->stack setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->stack];
    [self->stack setBackgroundColor:[UIColor redColor]];
    
    self->selfBattlefield = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight*2-gridHeight2, width-margin*4-cardWidth-textWidth, gridHeight)];
    self->selfBattlefield.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->selfBattlefield setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->selfBattlefield];
    [self->selfBattlefield setBackgroundColor:[UIColor greenColor]];
    
    self->selfLands = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight-gridHeight2, width-margin*4-cardWidth-textWidth, gridHeight2)];
    self->selfLands.contentSize = CGSizeMake((cardHeight2+margin)*maxColumns,gridHeight2);
    [self->selfLands setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->selfLands];
    [self->selfLands setBackgroundColor:[UIColor redColor]];
    
    self->scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight, width-margin*2, gridHeight)];
    self->scrollView.contentSize = CGSizeMake((cardWidth+margin)*maxColumns,gridHeight);
    [self->scrollView setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->scrollView];
    //[self->scrollView setBackgroundColor:[UIColor greenColor]];
    self->scrollView.layer.borderColor = [UIColor blueColor].CGColor;
    self->scrollView.layer.borderWidth = 3.0;
    
    self->popupMask = [[UIView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    self->popupMask.alpha = 0.5;
    [self->popupMask setBackgroundColor:[UIColor blackColor]];
    self->popupImage = [[UIImageView alloc] initWithFrame:CGRectMake((width-popupWidth)/2, (height-popupHeight)/2, popupWidth, popupHeight)];
    
    const CGFloat comp[3] = {0.5,0.5,0.5};
    CGColorRef col = CGColorCreate(CGColorSpaceCreateDeviceRGB(), comp);
    self->selfHP= [[UITextView alloc] initWithFrame:CGRectMake(width-margin-textWidth, height-margin-gridHeight*2-gridHeight2, textWidth, textHeight)];
    [selfHP.layer setBorderColor:col];
    [selfHP.layer setBorderWidth:2.0];
    selfHP.layer.cornerRadius = 5;
    selfHP.clipsToBounds = YES;
    selfHP.editable = NO;
    [selfHP setFont: [UIFont boldSystemFontOfSize:15]];
    [self.view addSubview:selfHP];
    self->opponentHP= [[UITextView alloc] initWithFrame:CGRectMake(margin*2+cardWidth,topmargin, textWidth, textHeight)];
    [opponentHP.layer setBorderColor:col];
    [opponentHP.layer setBorderWidth:2.0];
    opponentHP.layer.cornerRadius = 5;
    opponentHP.clipsToBounds = YES;
    opponentHP.editable = NO;
    [opponentHP setFont: [UIFont boldSystemFontOfSize:15]];
    [self.view addSubview:opponentHP];
    
    UIButton* pauseButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [pauseButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin, buttonWidth, buttonHeight)];
    [pauseButton setTitle:@"Pause" forState:UIControlStateNormal];
    //[pauseButton setBackgroundColor:[UIColor grayColor]];
    [pauseButton addTarget:self action:@selector(onPause:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:pauseButton];
    UIButton* endturnButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [endturnButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin+margin+buttonHeight, buttonWidth, buttonHeight)];
    [endturnButton setTitle:@"End Turn" forState:UIControlStateNormal];
    //[endturnButton setBackgroundColor:[UIColor grayColor]];
    [endturnButton addTarget:self action:@selector(onEndturn:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:endturnButton];
    
    UITapGestureRecognizer *recog1 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog1.numberOfTapsRequired = 2;
    [self->scrollView addGestureRecognizer:recog1];
    
    UITapGestureRecognizer *recog2 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(exitTap:)];
    [self->popupMask addGestureRecognizer:recog2];
    
    UILongPressGestureRecognizer *recog3 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->scrollView addGestureRecognizer:recog3];
    
    //UITapGestureRecognizer *recog4 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapDeck:)];
    //[self->selfDeck addGestureRecognizer:recog4];
    
    UITapGestureRecognizer *recog5 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog5.numberOfTapsRequired = 2;
    [self->opponentLands addGestureRecognizer:recog5];
    UITapGestureRecognizer *recog6 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog6.numberOfTapsRequired = 2;
    [self->opponentBattlefield addGestureRecognizer:recog6];
    UITapGestureRecognizer *recog7 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog7.numberOfTapsRequired = 2;
    [self->stack addGestureRecognizer:recog7];
    UITapGestureRecognizer *recog8 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog8.numberOfTapsRequired = 2;
    [self->selfBattlefield addGestureRecognizer:recog8];
    UITapGestureRecognizer *recog9 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog9.numberOfTapsRequired = 2;
    [self->selfLands addGestureRecognizer:recog9];
    
    UILongPressGestureRecognizer *recog10 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->selfBattlefield addGestureRecognizer:recog10];
    UILongPressGestureRecognizer *recog11 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->selfLands addGestureRecognizer:recog11];
    
    self->mulliganAlert = [[UIAlertView alloc] initWithTitle:nil message:@"Mulligan?" delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Yes",nil];
    
    cd = loadCardData();
    self->player = newGame();

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)displayToastWithMessage:(NSString *)toastMessage
{
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        UIWindow * keyWindow = [[UIApplication sharedApplication] keyWindow];
        UILabel *toastView = [[UILabel alloc] init];
        toastView.text = toastMessage;
        //toastView.font = [MYUIStyles getToastHeaderFont];
        //toastView.textColor = [MYUIStyles getToastTextColor];
        //toastView.backgroundColor = [[MYUIStyles getToastBackgroundColor] colorWithAlphaComponent:0.9];
        toastView.textColor = [UIColor whiteColor];
        toastView.backgroundColor = [UIColor blackColor];
        toastView.textAlignment = NSTextAlignmentCenter;
        //toastView.frame = CGRectMake(0.0, 0.0, keyWindow.frame.size.width/2.0, 100.0);
        CGFloat toastwidth = toastMessage.length * 10 + margin*2;
        toastView.frame = CGRectMake((width-toastwidth)/2, margin, toastwidth, buttonHeight);
        toastView.layer.cornerRadius = 10;
        toastView.layer.masksToBounds = YES;
        //toastView.center = keyWindow.center;
        
        [keyWindow addSubview:toastView];
        
        [UIView animateWithDuration: 3.0f
                              delay: 0.0
                            options: UIViewAnimationOptionCurveEaseOut
                         animations: ^{
                             toastView.alpha = 0.0;
                         }
                         completion: ^(BOOL finished) {
                             [toastView removeFromSuperview];
                         }
         ];
    }];
}

- (void) doubleTap: (UITapGestureRecognizer*) gesture {
    unsigned long idx;
    CGPoint p = [gesture locationInView:[gesture view]];
    if ([gesture view] == self->scrollView) {
        idx = p.x / (self->cardWidth + self->margin);
        if (idx >= self->images.count)
            idx = self->images.count - 1;
        [self->popupImage setImage:self->images[idx]];
    } else if ([gesture view] == self->selfLands) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfLandsViews.count)
            idx = self->selfLandsViews.count - 1;
        [self->popupImage setImage:self->selfLandsImages[idx]];
        self->currentPermanent = self->player->lands->entries[idx];
    } else if ([gesture view] == self->selfBattlefield) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        [self->popupImage setImage:self->selfBattlefieldImages[idx]];
        self->currentPermanent = self->player->battlefield->entries[idx];
    }
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
    CGPoint p = [gesture locationInView:[gesture view]];
    unsigned long idx;
    if ([gesture view] == self->scrollView) {
        idx = p.x / (self->cardWidth + self->margin);
        if (MTGPlayer_playCard(self->player, (int)idx, self->buffer)) {
            displayHand(self->player->hand);
            displayStats(self->player->hp,self->player->library->size, self->player->hand->size, true);
            displayLands(self->player->lands, true);
            displayBattlefield(self->player->battlefield, true);
        } else {
            message(self->buffer);
        }
    } else if ([gesture view] == self->selfLands) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfLandsViews.count)
            idx = self->selfLandsViews.count - 1;
        self->currentPermanent = self->player->lands->entries[idx];
        if (!self->currentPermanent->is_tapped)
            MTGPlayer_tap(self->player, self->currentPermanent);
        displayLands(self->player->lands, true);
    } else if ([gesture view] == self->selfBattlefield) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        self->currentPermanent = self->player->battlefield->entries[idx];
        if (!self->currentPermanent->is_tapped)
            MTGPlayer_tap(self->player, self->currentPermanent);
        displayBattlefield(self->player->battlefield, true);
    }
}

- (void) onEndturn: (id)sender{
    newTurn();
}

- (void) onTap: (id) sender{
    MTGPlayer_tap(self->player, self->currentPermanent);
    [self->popupMask removeFromSuperview];
    [self->popupImage removeFromSuperview];

}

- (void) onPause:(id)sender {
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"New Game",@"Tap All Lands", nil];
    [sheet showInView:self.view];
}

- (void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    switch (buttonIndex) {
        case 0:
            endGame();
            self->player = newGame();
            break;
        case 1:
            for (unsigned int i=0;i<self->player->lands->size;i++) {
                Permanent* p = self->player->lands->entries[i];
                if (!p->is_tapped) {
                    MTGPlayer_tap(self->player, p);
                }
            }
            displayLands(self->player->lands, true);
            break;
        default:
            break;
    }
}

- (void) alertView:(UIAlertView*) alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (alertView == self->mulliganAlert) {
        if (buttonIndex==1) {
            apply_mulligan(self->player);
            if (self->player->hand->size > 0)
                mulligan();
        } else {
            startGame();
        }
    }
}

@end

void displayHand(List* cards) {
    
    [viewController->images removeAllObjects];
    for (UIImageView* iv in viewController->views) {
        [iv removeFromSuperview];
    }
    [viewController->views removeAllObjects];
    
    int numCards = cards->size > viewController->maxColumns ? viewController->maxColumns : cards->size;
    viewController->scrollView.contentSize = CGSizeMake((viewController->cardWidth+viewController->margin)*numCards,viewController->cardHeight);
    
    NSString* extension = @".jpg";
    for (unsigned int i=0;i<numCards;i++) {
        MTGCard* card = cards->entries[i];
        const char* tag = card->name;
        [viewController->views addObject:[[UIImageView alloc] initWithFrame:CGRectMake((viewController->margin+viewController->cardWidth)*i,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        [viewController->images addObject:[UIImage imageNamed:fileName]];
        [viewController->views[i] setImage: viewController->images[i]];
        [viewController->scrollView addSubview:viewController->views[i]];
    }
    
}

void displayStats(int hp,int librarySize,int handSize, bool selfOrOpponent) {
    NSString* hpString = [NSString stringWithFormat:@"HP: %d\nLibrary: %d\nHand: %d",hp,librarySize,handSize];
    if (selfOrOpponent) {
        [viewController->selfHP setText:hpString];
    } else {
        [viewController->opponentHP setText:hpString];
    }
}

void displayLands(List* permanents, bool selfOrOpponent) {
    UIScrollView* currentScrollView;
    NSMutableArray* currentImages;
    NSMutableArray* currentViews;
    if (selfOrOpponent) {
        currentScrollView = viewController->selfLands;
        currentImages = viewController->selfLandsImages;
        currentViews = viewController->selfLandsViews;
    } else {
        currentScrollView = viewController->opponentLands;
        currentImages = viewController->opponentLandsImages;
        currentViews = viewController->opponentLandsViews;
    }
    
    [currentImages removeAllObjects];
    for (UIImageView* iv in currentViews) {
        [iv removeFromSuperview];
    }
    [currentViews removeAllObjects];
    
    int numCards = permanents->size > viewController->maxColumns ? viewController->maxColumns : permanents->size;
    currentScrollView.contentSize = CGSizeMake((viewController->cardHeight2+viewController->margin)*numCards,viewController->cardHeight2);
    
    NSString* extension = @".jpg";
    CGFloat x = 0;
    CGFloat tappedMargin = (viewController->gridHeight2 - viewController->cardWidth2)/2;
    for (unsigned int i=0;i<numCards;i++) {
        Permanent* p = permanents->entries[i];
        MTGCard* card = p->source;
        const char* tag = card->name;
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        if (p->is_tapped) {
            [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,tappedMargin, viewController->cardHeight2, viewController->cardWidth2)]];
            UIImage * portrait = [UIImage imageNamed: fileName];
            UIImage * landscape = [[UIImage alloc] initWithCGImage: portrait.CGImage scale: 1.0 orientation: UIImageOrientationRight];
            [currentImages addObject:landscape];
            x += viewController->cardHeight2 + viewController->margin;
        } else {
            [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,viewController->margin, viewController->cardWidth2, viewController->cardHeight2)]];
            [currentImages addObject:[UIImage imageNamed:fileName]];
            x += viewController->cardWidth2 + viewController->margin;
        }
        [currentViews[i] setImage: currentImages[i]];
        [currentScrollView addSubview:currentViews[i]];
    }
}

void displayBattlefield(List* permanents, bool selfOrOpponent) {
    UIScrollView* currentScrollView;
    NSMutableArray* currentImages;
    NSMutableArray* currentViews;
    if (selfOrOpponent) {
        currentScrollView = viewController->selfBattlefield;
        currentImages = viewController->selfBattlefieldImages;
        currentViews = viewController->selfBattlefieldViews;
    } else {
        currentScrollView = viewController->opponentBattlefield;
        currentImages = viewController->opponentBattlefieldImages;
        currentViews = viewController->opponentBattlefieldViews;
    }
    
    [currentImages removeAllObjects];
    for (UIImageView* iv in currentViews) {
        [iv removeFromSuperview];
    }
    [currentViews removeAllObjects];
    
    int numCards = permanents->size > viewController->maxColumns ? viewController->maxColumns : permanents->size;
    currentScrollView.contentSize = CGSizeMake((viewController->cardHeight+viewController->margin)*numCards,viewController->cardHeight);
    
    NSString* extension = @".jpg";
    CGFloat x = 0;
    CGFloat tappedMargin = (viewController->gridHeight - viewController->cardWidth)/2;
    for (unsigned int i=0;i<numCards;i++) {
        Permanent* p = permanents->entries[i];
        MTGCard* card = p->source;
        const char* tag = card->name;
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        if (p->is_tapped) {
            [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,tappedMargin, viewController->cardHeight, viewController->cardWidth)]];
            UIImage * portrait = [UIImage imageNamed: fileName];
            UIImage * landscape = [[UIImage alloc] initWithCGImage: portrait.CGImage scale: 1.0 orientation: UIImageOrientationRight];
            [currentImages addObject:landscape];
            x += viewController->cardHeight + viewController->margin;
        } else {
            [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
            [currentImages addObject:[UIImage imageNamed:fileName]];
            x += viewController->cardWidth + viewController->margin;
        }
        [currentViews[i] setImage: currentImages[i]];
        [currentScrollView addSubview:currentViews[i]];
    }
}

void saveDeck(char* name,List* cards){
    
}

void loadDeck(char* name,List* cards) {
    
}

void startTurn(MTGPlayer* player) {

}

void discardToSeven(MTGPlayer* player){
    sprintf(viewController->buffer,"Please discard down to 7 cards\n");
    message(viewController->buffer);
}

void mulligan() {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [viewController->mulliganAlert show];
    });
}

void message(char* msg) {
    NSString *err = [NSString stringWithUTF8String:msg];
    //UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" message:err delegate:viewController cancelButtonTitle:@"OK" otherButtonTitles:nil];
    //dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        //[alert show];
    //});
    [viewController displayToastWithMessage:err];

}
