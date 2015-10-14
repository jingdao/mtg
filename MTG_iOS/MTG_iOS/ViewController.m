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
HashTable* cdt;
List* categories;
ViewController* viewController;
extern MTGPlayer* currentPlayer;
extern MTGPlayer* player2;

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
    self->coverWidth = self->coverImage.size.width;
    self->coverHeight = self->coverImage.size.height;
    self->gridHeight = (height-margin-topmargin)/16 * 3;
    self->cardHeight = self->gridHeight - margin*2;
    self->cardWidth = cardHeight/coverHeight*coverWidth;
    self->gridHeight2 = (height-margin-topmargin)/8;
    self->cardHeight2 = self->gridHeight2 - margin*2;
    self->cardWidth2 = cardHeight2/coverHeight*coverWidth;
    self->popupWidth = width/3;
    self->popupHeight = popupWidth/coverWidth*coverHeight;
    self->labelWidth = cardHeight/2;
    self->labelHeight = labelWidth/2;
    self->textWidth = 100;
    self->textHeight = 200;
    self->buttonWidth = 100;
    self->buttonHeight = 30;
    self->stepperWidth = 100;
    self->stepperHeight = 35;
    self->cacheImages = false;
    self->deck_index = 0;
    self->selfDeck = [[UIImageView alloc] initWithFrame:CGRectMake(width-margin*2-cardWidth-textWidth,height-margin-gridHeight*2-gridHeight2,cardWidth,cardHeight)];
    [self->selfDeck setImage: self->coverImage];
    self->selfDeck.userInteractionEnabled = YES;
    [self.view addSubview:self->selfDeck];
    self->opponentDeck = [[UIImageView alloc] initWithFrame:CGRectMake(margin,topmargin,cardWidth,cardHeight)];
    [self->opponentDeck setImage: self->coverImage];
    self->opponentDeck.userInteractionEnabled = YES;
    [self.view addSubview:self->opponentDeck];
    self.view.backgroundColor = [UIColor colorWithRed:0.9 green:0.9 blue:0.9 alpha:1.0];
    
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
    self->selfLabels = [[NSMutableArray alloc]init];
    self->opponentLabels = [[NSMutableArray alloc]init];
    self->views = [[NSMutableArray alloc] init];
    self->images = [[NSMutableArray alloc] init];
    self->messageQueue = [[NSMutableArray alloc] init];
    self->commandQueue = [[NSMutableArray alloc] init];


    self->opponentLands = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin, width-margin*5-cardWidth-textWidth-buttonWidth, gridHeight2)];
    self->opponentLands.contentSize = CGSizeMake((cardHeight2+margin)*maxColumns,gridHeight2);
    [self->opponentLands setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->opponentLands];
    //[self->opponentLands setBackgroundColor:[UIColor redColor]];
    
    self->opponentBattlefield = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin+gridHeight2, width-margin*5-cardWidth-textWidth-buttonWidth, gridHeight)];
    self->opponentBattlefield.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->opponentBattlefield setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->opponentBattlefield];
    //[self->opponentBattlefield setBackgroundColor:[UIColor greenColor]];
    
    self->stack = [[UIScrollView alloc] initWithFrame:CGRectMake(margin*3+cardWidth+textWidth,topmargin+gridHeight+gridHeight2, width-margin*6-cardWidth*2-textWidth*2, gridHeight)];
    self->stack.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->stack setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->stack];
    //[self->stack setBackgroundColor:[UIColor redColor]];
    
    self->selfBattlefield = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight*2-gridHeight2, width-margin*4-cardWidth-textWidth, gridHeight)];
    self->selfBattlefield.contentSize = CGSizeMake((cardHeight+margin)*maxColumns,gridHeight);
    [self->selfBattlefield setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->selfBattlefield];
    //[self->selfBattlefield setBackgroundColor:[UIColor greenColor]];
    
    self->selfLands = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight-gridHeight2, width-margin*4-cardWidth-textWidth, gridHeight2)];
    self->selfLands.contentSize = CGSizeMake((cardHeight2+margin)*maxColumns,gridHeight2);
    [self->selfLands setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->selfLands];
    //[self->selfLands setBackgroundColor:[UIColor redColor]];
    
    self->scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(margin, height-margin-gridHeight, width-margin*2, gridHeight)];
    self->scrollView.contentSize = CGSizeMake((cardWidth+margin)*maxColumns,gridHeight);
    [self->scrollView setShowsHorizontalScrollIndicator:NO];
    [self.view addSubview:self->scrollView];
    //[self->scrollView setBackgroundColor:[UIColor greenColor]];
    //self->scrollView.layer.borderColor = [UIColor blueColor].CGColor;
    //self->scrollView.layer.borderWidth = 3.0;
    
   
    
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
    [selfHP setFont: [UIFont boldSystemFontOfSize:12]];
    [self.view addSubview:selfHP];
    self->opponentHP= [[UITextView alloc] initWithFrame:CGRectMake(margin*2+cardWidth,topmargin, textWidth, textHeight)];
    [opponentHP.layer setBorderColor:col];
    [opponentHP.layer setBorderWidth:2.0];
    opponentHP.layer.cornerRadius = 5;
    opponentHP.clipsToBounds = YES;
    opponentHP.editable = NO;
    [opponentHP setFont: [UIFont boldSystemFontOfSize:12]];
    [self.view addSubview:opponentHP];
    
    UIButton* pauseButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [pauseButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin, buttonWidth, buttonHeight)];
    [pauseButton setTitle:@"Pause" forState:UIControlStateNormal];
    //[pauseButton setBackgroundColor:[UIColor grayColor]];
    [pauseButton addTarget:self action:@selector(onPause:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:pauseButton];
    endturnButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [endturnButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin+margin+buttonHeight, buttonWidth, buttonHeight)];
    [endturnButton setTitle:@"End Turn" forState:UIControlStateNormal];
    //[endturnButton setBackgroundColor:[UIColor grayColor]];
    [endturnButton addTarget:self action:@selector(onEndturn:) forControlEvents:UIControlEventTouchUpInside];
    endturnButton.enabled=false;
    [self.view addSubview:endturnButton];
    attackButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [attackButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin+(margin+buttonHeight)*2, buttonWidth, buttonHeight)];
    [attackButton setTitle:@"Attack" forState:UIControlStateNormal];
    [attackButton addTarget:self action:@selector(onAttack:) forControlEvents:UIControlEventTouchUpInside];
    attackButton.enabled=false;
    [self.view addSubview:attackButton];
    confirmButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [confirmButton setFrame:CGRectMake(width-buttonWidth-margin, topmargin+(margin+buttonHeight)*3, buttonWidth, buttonHeight)];
    [confirmButton setTitle:@"Confirm" forState:UIControlStateNormal];
    [confirmButton addTarget:self action:@selector(onConfirm:) forControlEvents:UIControlEventTouchUpInside];
    confirmButton.enabled = false;
    [self.view addSubview:confirmButton];
    
    UITapGestureRecognizer *recog1 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog1.numberOfTapsRequired = 2;
    [self->scrollView addGestureRecognizer:recog1];
    UITapGestureRecognizer *recog2 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(exitTap:)];
    [self->popupMask addGestureRecognizer:recog2];
    UILongPressGestureRecognizer *recog3 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->scrollView addGestureRecognizer:recog3];
    
    UITapGestureRecognizer *recog4 = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doubleTap:)];
    recog4.numberOfTapsRequired = 2;
    [self->stack addGestureRecognizer:recog4];
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
    UILongPressGestureRecognizer *recog12 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->opponentBattlefield addGestureRecognizer:recog12];
    UILongPressGestureRecognizer *recog13 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->opponentLands addGestureRecognizer:recog13];
    UILongPressGestureRecognizer *recog14 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->opponentDeck addGestureRecognizer:recog14];
    UILongPressGestureRecognizer *recog15 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->selfDeck addGestureRecognizer:recog15];
    
    self->mulliganAlert = [[UIAlertView alloc] initWithTitle:nil message:@"Mulligan?" delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Yes",nil];
    self->winnerAlert = [[UIAlertView alloc] initWithTitle:nil message:nil delegate:viewController cancelButtonTitle:@"New Game" otherButtonTitles:nil];
    self->manaAlert = [[UIView alloc] initWithFrame: CGRectMake((width-stepperWidth*2-margin*2)/2, (height-margin*4-stepperHeight*5)/2, margin*4+stepperWidth*2, margin*4+stepperHeight*5)];
    manaAlert.backgroundColor = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:1.0];
    manaAlert.layer.cornerRadius = 10.0;
    self->manaLabel = [[UITextView alloc] initWithFrame:CGRectMake(margin*2, margin*2, stepperWidth,margin*2+stepperHeight*4)];
    manaLabel.editable = false;
    manaLabel.backgroundColor = manaAlert.backgroundColor;
    [manaLabel setFont: [UIFont systemFontOfSize:15]];
    [manaAlert addSubview:manaLabel];
    manaButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [manaButton setFrame:CGRectMake(margin*2, margin*4+stepperHeight*4, stepperWidth, stepperHeight)];
    [manaButton setTitle:@"Confirm" forState:UIControlStateNormal];
    [manaButton addTarget:self action:@selector(onManaButton:) forControlEvents:UIControlEventTouchUpInside];
    [manaAlert addSubview:manaButton];
    for (int i=1;i<=5;i++) {
        UIStepper* stepper = [[UIStepper alloc] initWithFrame:CGRectMake(stepperWidth+margin*2,margin+stepperHeight*(i-1),stepperWidth,stepperHeight)];
        stepper.tag=i;
        [stepper addTarget:self action:@selector(stepperChanged:) forControlEvents:UIControlEventValueChanged];
        [manaAlert addSubview:stepper];
    }
    
    deckSheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil otherButtonTitles:@"Custom",@"Dragon's Hoard", @"Hit the Ground Running",@"Infernal Intervention",@"Price of Glory",@"Will of the Masses",nil];
    selectSheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:nil destructiveButtonTitle: nil otherButtonTitles:nil];
    self.deckController = [[DeckController alloc] init];
    self.deckController->coverWidth = self->coverWidth;
    self.deckController->coverHeight = self->coverHeight;
    self.deckController->buttonWidth = self->buttonWidth;
    self.deckController->buttonHeight = self->buttonHeight;
    self.deckController->cacheImages = self->cacheImages;
    self.deckController->deck_index = self->deck_index;
    self.deckPopover = [[UIPopoverController alloc] initWithContentViewController:self.deckController];
    self.deckPopover.popoverContentSize = CGSizeMake(width-margin*2,coverHeight*2 + margin*6 + buttonHeight);
    self.deckPopover.delegate = self;
    
    cd = loadCardData();
    loadCardDataTable();
    loadAbilities();
    self->player = newGame(deck_index);

}

- (void)changeMode: (Mode) m {
    switch (m) {
        case DISCARD:
            confirmButton.enabled=true;
            endturnButton.enabled=false;
            attackButton.enabled=false;
            break;
        case VIEW:
            confirmButton.enabled=true;
            endturnButton.enabled=false;
            attackButton.enabled=false;
            break;
        case ATTACK:
            attackButton.enabled = false;
            endturnButton.enabled = false;
            confirmButton.enabled = true;
            break;
        case SELECT_TARGET:
            attackButton.enabled = false;
            endturnButton.enabled = false;
            confirmButton.enabled = true;
            break;
        case SELECT_CARDS:
            attackButton.enabled = false;
            endturnButton.enabled = false;
            confirmButton.enabled = true;
            break;
        case STACK:
            attackButton.enabled = false;
            endturnButton.enabled = false;
            confirmButton.enabled = true;
            break;
        case WAIT:
            attackButton.enabled = false;
            endturnButton.enabled = false;
            confirmButton.enabled = true;
            break;
        case NONE:
            attackButton.enabled = canAttack;
            endturnButton.enabled = true;
            confirmButton.enabled = false;
            break;
        default:
            break;
    }
    if (m == STACK && currentPlayer != player)
        mode = WAIT;
    else
        mode = m;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)showToast {
    [[NSOperationQueue mainQueue] addOperationWithBlock:^ {
        UIWindow * keyWindow = [[UIApplication sharedApplication] keyWindow];
        UILabel *toastView = [[UILabel alloc] init];
        NSString* toastMessage = messageQueue[0];
        [messageQueue removeObjectAtIndex:0];
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
        
        if (messageQueue.count > 0)
            [UIView animateWithDuration: 1.0f delay: 0.0 options: UIViewAnimationOptionCurveEaseOut
                             animations: ^{    toastView.alpha = 0.0;}
                             completion: ^(BOOL finished) {
                                 [toastView removeFromSuperview];
                                 if (messageQueue.count > 0) [self showToast];
                             }
             ];
        else
            [UIView animateWithDuration: 2.0f delay: 0.0 options: UIViewAnimationOptionCurveEaseOut
                             animations: ^{    toastView.alpha = 0.0;}
                             completion: ^(BOOL finished) {
                                 [toastView removeFromSuperview];
                             }
             ];
    }];
}

- (void)displayToastWithMessage:(NSString *)toastMessage
{
    [viewController->messageQueue addObject:toastMessage];
    if (viewController->messageQueue.count == 1)
        [self showToast];
    
}

- (UIImage*) loadImage: (NSString*)name cached: (BOOL) isCached {
    if (isCached)
        return [UIImage imageNamed:name];
    else
        //return [UIImage imageWithContentsOfFile:name];
        return [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:name ofType:nil]];
}

- (void) disableHighlight {
    if (selfBattlefield.layer.borderWidth > 0) [self toggleHighlight:selfBattlefield];
    if (opponentBattlefield.layer.borderWidth > 0) [self toggleHighlight:opponentBattlefield];
    if (selfLands.layer.borderWidth > 0) [self toggleHighlight:selfLands];
    if (opponentLands.layer.borderWidth > 0) [self toggleHighlight:opponentLands];
    if (selfDeck.layer.borderWidth > 0) [self toggleCard:selfDeck];
    if (opponentDeck.layer.borderWidth > 0) [self toggleCard:opponentDeck];
}

- (void) toggleHighlight: (UIScrollView*) sv {
    if (sv.layer.borderWidth > 0) {
        sv.layer.borderWidth = 0;
    } else {
        sv.layer.borderColor = [UIColor blueColor].CGColor;
        sv.layer.borderWidth = 3.0;
    }
}

- (void) toggleCard: (UIImageView*) iv {
    if (iv.layer.borderWidth > 0) {
        iv.layer.borderWidth = 0;
    } else {
        iv.layer.borderColor = [UIColor redColor].CGColor;
        iv.layer.borderWidth = 5.0;
    }
}

- (void) doubleTap: (UITapGestureRecognizer*) gesture {
    unsigned long idx;
    CGPoint p = [gesture locationInView:[gesture view]];
    UIImage* im = NULL;
    if ([gesture view] == self->scrollView && player->hand->size > 0) {
        idx = p.x / (self->cardWidth + self->margin);
        if (idx >= self->images.count)
            idx = self->images.count - 1;
        im=images[idx];
    } else if ([gesture view] == self->selfLands && selfLandsViews.count > 0) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfLandsViews.count)
            idx = self->selfLandsViews.count - 1;
        im=selfLandsImages[idx];
    } else if ([gesture view] == self->selfBattlefield && selfBattlefieldViews.count > 0) {
        for (idx=0;idx<self->selfBattlefieldViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        im=selfBattlefieldImages[idx];
    } else if ([gesture view] == self->opponentLands && opponentLandsViews.count > 0) {
        for (idx=0;idx<self->opponentLandsViews.count;idx++) {
            UIImageView *imv = self->opponentLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentLandsViews.count)
            idx = self->opponentLandsViews.count - 1;
        im=opponentLandsImages[idx];
    } else if ([gesture view] == self->opponentBattlefield && opponentBattlefieldViews.count > 0) {
        for (idx=0;idx<self->opponentBattlefieldViews.count;idx++) {
            UIImageView *imv = self->opponentBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentBattlefieldViews.count)
            idx = self->opponentBattlefieldViews.count - 1;
        im = self->opponentBattlefieldImages[idx];
    } else if ([gesture view] == stack && stackViews.count > 0) {
        for (idx=0;idx<self->stackViews.count;idx++) {
            UIImageView *imv = self->stackViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->stackViews.count)
            idx = self->stackViews.count - 1;
        im = self->stackImages[idx];
    }
    if (im && ![im isEqual:[NSNull null]]) {
        [self->popupImage setImage:im];
        [self.view addSubview:self->popupMask];
        [self.view addSubview:self->popupImage];
    }
}
     
- (void) exitTap: (UITapGestureRecognizer*) gesture {
    if (self->mode != MANA) {
        [self->popupMask removeFromSuperview];
        [self->popupImage removeFromSuperview];
    }
}

- (void) longpress: (UILongPressGestureRecognizer*) gesture {
    if (!(gesture.state==UIGestureRecognizerStateBegan))
        return;
    CGPoint p = [gesture locationInView:[gesture view]];
    long idx;
    if ([gesture view] == self->scrollView) {
        idx = p.x / (self->cardWidth + self->margin);
        if (idx >= self->images.count)
            idx = self->images.count - 1;
        if (idx < 0)
            return;
        if (mode==DISCARD || mode==SELECT_CARDS) {
            [self toggleCard:self->views[idx]];
        } else if (mode==WAIT || mode==STACK) {
            MTGCard* card = player->hand->entries[idx];
            if (card->subtypes.is_instant) {
                currentPermanent = MTGPlayer_playCard(self->player, (int)idx, self->buffer);
                if (currentPermanent) {
                    displayHand(self->player->hand);
                    displayStats(self->player->hp,self->player->library->size, self->player->hand->size, self->player->graveyard->size,self->player->exile->size,self->player->mana,true);
                    if (pendingMana==0) {
                        if (Event_onPlay(currentPermanent))
                            [self changeMode:STACK];
                    }
                } else message(buffer);
            }
        } else if (mode==NONE){
            currentPermanent = MTGPlayer_playCard(self->player, (int)idx, self->buffer);
            if (currentPermanent) {
                displayHand(self->player->hand);
                displayStats(self->player->hp,self->player->library->size, self->player->hand->size, self->player->graveyard->size,self->player->exile->size,self->player->mana,true);
                displayLands(self->player->lands, true);
                displayBattlefield(self->player->battlefield, true);
                if (pendingMana == 0)
                    if (Event_onPlay(currentPermanent))
                        [self changeMode:STACK];
            } else {
                message(self->buffer);
            }
        }
    } else if ([gesture view] == self->selfLands && selfLandsViews.count > 0) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfLandsViews.count)
            idx = self->selfLandsViews.count - 1;
        currentPermanent = player->lands->entries[idx];
        if (mode==NONE) {
            if (!self->currentPermanent->is_tapped)
                MTGPlayer_tap(self->player, self->currentPermanent);
            displayLands(self->player->lands, true);
            displayStats(self->player->hp, self->player->library->size, self->player->hand->size,self->player->graveyard->size ,self->player->exile->size ,self->player->mana, self);
        } else if (mode==SELECT_TARGET && selfLands.layer.borderWidth > 0) {
            [self disableHighlight];
            currentEquipment->target = currentPermanent;
            [self changeMode:STACK];
            [self onConfirm:NULL];
        }
    } else if ([gesture view] == self->selfBattlefield && selfBattlefieldViews.count > 0) {
        for (idx=0;idx<self->selfBattlefieldViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        currentPermanent = MTGPlayer_getBattlefieldPermanent(player->battlefield, (unsigned int)idx);
        if (mode==ATTACK) {
            if (!self->currentPermanent->subtypes.is_creature)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s is not a creature!",self->currentPermanent->name]];
            else if (self->currentPermanent->has_summoning_sickness && !currentPermanent->subtypes.is_haste)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s has summoning sickness!",self->currentPermanent->name]];
            else if (self->currentPermanent->is_tapped || currentPermanent->subtypes.is_defender || ! currentPermanent->canAttack)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s cannot attack!",self->currentPermanent->name]];
            else {
                self->currentPermanent->has_attacked= !self->currentPermanent->has_attacked;
                [self toggleCard:selfBattlefieldViews[idx]];
            }
        } else if (mode==BLOCK) {
            Permanent* attacker = attackerList->entries[block_index];
            if (!self->currentPermanent->subtypes.is_creature)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s is not a creature!",self->currentPermanent->name]];
            else if (self->currentPermanent->is_tapped || self->currentPermanent->has_blocked)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s cannot block!",self->currentPermanent->name]];
            else if (attacker->subtypes.is_flying && !currentPermanent->subtypes.is_flying && !currentPermanent->subtypes.is_reach)
                [self displayToastWithMessage:@"Can only block with creatures with flying/reach!"];
            else if (attacker->subtypes.is_intimidate && !currentPermanent->subtypes.is_artifact && !Permanent_sameColor(attacker, currentPermanent))
                [self displayToastWithMessage:@"Blocking creature must be artifact or share a color!"];
            else if (currentPermanent->has_attacked) {
                currentPermanent->has_attacked = false;
                List* blockers = blockersList->entries[block_index];
                RemoveListObject(blockers, player->battlefield->entries[idx]);
                [self toggleCard:selfBattlefieldViews[idx]];
            } else {
                self->currentPermanent->has_attacked= true;
                [self toggleCard:selfBattlefieldViews[idx]];
                List* blockers = blockersList->entries[block_index];
                AppendToList(blockers, currentPermanent);
            }
        } else if (mode==SELECT_TARGET) {
            [self disableHighlight];
            if (target_index == 3)
                currentEquipment->target3 = currentPermanent;
            else if (target_index == 2)
                currentEquipment->target2 = currentPermanent;
            else
                currentEquipment->target = currentPermanent;
            [self changeMode:STACK];
            [self onConfirm:NULL];
        } else if (mode==WAIT || mode==STACK) {
            if (currentPermanent->subtypes.has_instant) {
                if (MTGPlayer_tap(self->player, self->currentPermanent)) {
                    if (MTGPlayer_activateAbility(player, currentPermanent,buffer)) {
                        displayBattlefield(self->player->battlefield, true);
                        if (pendingMana == 0)
                            if (Event_onPlay(currentPermanent))
                                [self changeMode:STACK];
                    } else
                        message(buffer);
                }
            }
        } else if (mode==NONE) {
            if (currentPermanent->subtypes.has_instant) {
                if (MTGPlayer_tap(self->player, self->currentPermanent)) {
                    if (MTGPlayer_activateAbility(player, currentPermanent,buffer)) {
                        displayBattlefield(self->player->battlefield, true);
                        if (pendingMana == 0)
                            if (Event_onPlay(currentPermanent))
                                [self changeMode:STACK];
                    } else
                        message(buffer);
                }
            }
        }
    } else if ([gesture view] == self->opponentBattlefield && opponentBattlefieldViews.count > 0) {
        for (idx=0;idx<self->opponentBattlefieldViews.count;idx++) {
            UIImageView *imv = self->opponentBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentBattlefieldViews.count)
            idx = self->opponentBattlefieldViews.count - 1;
        currentPermanent = MTGPlayer_getBattlefieldPermanent(opponentPermanents, (unsigned int)idx);
        if (mode==SELECT_TARGET) {
            [self disableHighlight];
            if (target_index == 3)
                currentEquipment->target3 = currentPermanent;
            else if (target_index == 2)
                currentEquipment->target2 = currentPermanent;
            else
                currentEquipment->target = currentPermanent;
            [self changeMode:STACK];
            [self onConfirm:NULL];
        }
    } else if ([gesture view] == opponentLands && opponentLandsViews.count > 0) {
        for (idx=0;idx<self->opponentLandsViews.count;idx++) {
            UIImageView *imv = self->opponentLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentLandsViews.count)
            idx = self->opponentLandsViews.count - 1;
        currentPermanent = player2->lands->entries[idx];
        if (mode==SELECT_TARGET && opponentLands.layer.borderWidth > 0) {
            [self disableHighlight];
            currentEquipment->target = currentPermanent;
            [self changeMode:STACK];
            [self onConfirm:NULL];
        }
    } else if ([gesture view] == selfDeck && mode==SELECT_TARGET && selfDeck.layer.borderWidth > 0) {
        if (target_index == 3)
            currentEquipment->target3 = player->marker;
        else if (target_index == 2)
            currentEquipment->target2 = player->marker;
        else
            currentEquipment->target = player->marker;
        [self disableHighlight];
        [self changeMode:STACK];
        [self onConfirm:NULL];
    } else if ([gesture view] == opponentDeck && mode==SELECT_TARGET && opponentDeck.layer.borderWidth > 0) {
        if (target_index == 3)
            currentEquipment->target3 = player2->marker;
        else if (target_index == 2)
            currentEquipment->target2 = player2->marker;
        else
            currentEquipment->target = player2->marker;
        [self disableHighlight];
        [self changeMode:STACK];
        [self onConfirm:NULL];
    }
}

- (void) onEndturn: (id)sender{
    if (player->hand->size > 7) {
        discardCards(player,player->hand->size-7);
        [self onConfirm:NULL];
    } else {
        [self changeMode:WAIT];
        newTurn();
    }
}

- (void) onPause:(id)sender {
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"New Game",@"Tap All Lands", @"Attack All",@"View Graveyard",@"View Exile",@"Change Deck",@"Edit Deck",nil];
    [sheet showInView:self.view];
}

- (void) onAttack: (id)sender {
    [self toggleHighlight:selfBattlefield];
    [self changeMode:ATTACK];
}

- (void) onConfirm: (id)sender {
    if (mode == DISCARD) {
        int discarded=0;
        for (int i=0;i<views.count;i++) {
            UIImageView* iv = views[i];
            if (iv.layer.borderWidth > 0) {
                MTGPlayer_discard(player, i - discarded);
                discarded++;
            }
        }
        pendingDiscard -= discarded;
        displayHand(player->hand);
        displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->exile->size,player->mana,true);
        if (pendingDiscard > 0) {
            sprintf(buffer,"Discard %d cards\n",pendingDiscard);
            message(buffer);
            return;
        }

        if (stackViews.count > 0) {
            [self changeMode:STACK];
            return;
        }
    } else if (mode == VIEW) {
        [self toggleHighlight:scrollView];
        displayHand(currentPlayer->hand);
    } else if (mode == ATTACK) {
        List* permanentList = InitList();
        for (unsigned int i=0;i<player->battlefield->size;i++) {
            Permanent* p = player->battlefield->entries[i];
            if (p->has_attacked) {
                AppendToList(permanentList, p);
            }
        }
        if (!Event_attack(permanentList, buffer)) {
            message(buffer);
            DeleteList(permanentList);
            return;
        }
        for (unsigned int i=0;i<permanentList->size;i++) {
            Permanent* p = permanentList->entries[i];
            if (!p->subtypes.is_vigilance)
                p->is_tapped = true;
        }
        if (permanentList->size > 0) {
            resolveAttack(self->player, permanentList);
            canAttack = false;
            mode = WAITATTACK;
            DeleteList(permanentList);
            return;
        }
        displayBattlefield(player->battlefield, self);
        DeleteList(permanentList);
    } else if (mode == BLOCK) {
        int numBlockers = 0;
        for (unsigned int i=0;i<viewController->player->battlefield->size;i++) {
            Permanent* p = viewController->player->battlefield->entries[i];
            if (p->subtypes.is_creature && ! p->is_tapped)
                numBlockers++;
        }
        if (numBlockers > 0) {
            List* blockers = blockersList->entries[block_index];
            if (!MTGPlayer_block(attackerList->entries[block_index],blockers, buffer)) {
                message(buffer);
                return;
            }
            for (unsigned int i=0;i<player->battlefield->size;i++) {
                Permanent* p = player->battlefield->entries[i];
                if (p->has_attacked) {
                    p->has_blocked = true;
                    p->has_attacked = false;
                }
            }
        }
        block_index++;
        if (numBlockers==0 || block_index >= attackerList->size) {
            [self toggleHighlight:selfBattlefield];
            [self changeMode:WAIT];
            resolveBlock();
        } else if (numBlockers > 0){
            Permanent* p = attackerList->entries[block_index];
            int i;
            findTarget(p, &i);
            UIImageView* iv = opponentBattlefieldViews[i];
            [self toggleCard:iv];
        }
        return;
    } else if (mode == SELECT_TARGET) {
        [self disableHighlight];
        mode = STACK;
        return;
    } else if (mode == SELECT_CARDS) {
        int numTargeted = 0;
        for (int i=0;i<views.count;i++) {
            UIImageView* iv = views[i];
            if (iv.layer.borderWidth > 0) {
                if (numTargeted==0) {
                    currentEquipment->target = currentEquipment + i + 1;
                    numTargeted++;
                } else if (numTargeted==1) {
                    currentEquipment->target2 = currentEquipment + i + 1;
                    numTargeted++;
                }
                if (numTargeted>=2)
                    break;
            }
        }
        displayHand(player->hand);
        mode = STACK;
        return;
    } else if (mode == STACK) {
        if ([self continueAction])
            return;
        if (!resolveStack()) {
            [self changeMode:STACK];
            return;
        }
        if ([self continueAction])
            return;
    } else if (mode == WAIT) {
        if ([self continueAction])
            return;
        resolveAI();
        [self continueAction];
        return;
    } else if (mode == WAITATTACK) {
        if (!resolveBlock(player))
            return;
    }
    [self changeMode:NONE];
}

- (void) onManaButton: (id)sender {
    if (pendingMana < 0)
        currentPermanent->X = paidMana;
    memcpy(viewController->player->mana, manaBuffer, 6*sizeof(int));
    displayStats(player->hp,player->library->size,player->hand->size,player->graveyard->size,player->exile->size,player->mana,true);
    self->mode = NONE;
    [self->manaAlert removeFromSuperview];
    [self->popupMask removeFromSuperview];
    if (Event_onPlay(currentPermanent))
        [self changeMode:STACK];
}

- (bool) continueAction {
    if (commandQueue.count > 0) {
        void (^blockcommand)(void);
        blockcommand = commandQueue.firstObject;
        [commandQueue removeObjectAtIndex:0];
        blockcommand();
        return true;
    } else return false;
}

- (void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (actionSheet == self->deckSheet) {
        [self->popupMask removeFromSuperview];
        if (buttonIndex >= 0) {
            deck_index = (int)buttonIndex;
            attackButton.enabled=false;
            confirmButton.enabled=false;
            endturnButton.enabled=false;
            endGame();
            self->player = newGame(deck_index);
        }
        return;
    } else if (actionSheet == selectSheet) {
        [selectSheet removeFromSuperview];
        if (buttonIndex >= 0) {
            currentEquipment->target = currentEquipment + buttonIndex;
        }
        return;
    }
    switch (buttonIndex) {
        case 0:
        {
            attackButton.enabled=false;
            confirmButton.enabled=false;
            endturnButton.enabled=false;
            endGame();
            self->player = newGame(deck_index);
            break;
        }
        case 1:
        {
            if (mode == NONE) {
                for (unsigned int i=0;i<self->player->lands->size;i++) {
                    Permanent* p = self->player->lands->entries[i];
                    if (!p->is_tapped) {
                        MTGPlayer_tap(self->player, p);
                    }
                }
                displayLands(self->player->lands, true);
                displayStats(self->player->hp, self->player->library->size, self->player->hand->size, self->player->graveyard->size,self->player->exile->size ,self->player->mana, true);
            }
            break;
        }
        case 2:
        {
            if (mode == NONE && attackButton.enabled==true) {
                [self toggleHighlight:selfBattlefield];
                [self changeMode:ATTACK];
                for (int i=0;i<self->selfBattlefieldViews.count;i++) {
                    UIImageView* iv = self->selfBattlefieldViews[i];
                    Permanent* p = MTGPlayer_getBattlefieldPermanent(player->battlefield, i);
                    if (p->subtypes.is_creature&&(p->subtypes.is_haste||!p->has_summoning_sickness) && !p->is_tapped && !p->subtypes.is_defender) {
                        [self toggleCard:iv];
                        p->has_attacked = true;
                    }
                }
            }
            break;
        }
        case 3:
        {
            if (mode == NONE) {
                displayHand(currentPlayer->graveyard);
                [self toggleHighlight:scrollView];
                [self changeMode:VIEW];
            }
            break;
        }
        case 4:
        {
            if (mode == NONE) {
                displayHand(currentPlayer->exile);
                [self toggleHighlight:scrollView];
                [self changeMode:VIEW];
            }
            break;
        }
        case 5:
        {
            [self.view addSubview:self->popupMask];
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC / 4)), dispatch_get_main_queue(), ^{
                [self->deckSheet showInView:self.view];
            });
            break;
        }
        case 6: {
//            if (self.deckController->deck_index != self->deck_index) {
//                [self.deckController freeImages];
//                self.deckController->deck_index = self->deck_index;
//                [self.deckController showDeck];
//            }
            self.deckController->deck_index = self->deck_index;
            if (self.deckController.view) {
                [self.deckController showDeck];
                [self.deckController showStock];
                [self.deckController showCounts];
            }
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC / 2)), dispatch_get_main_queue(), ^{
                [self.deckPopover presentPopoverFromRect:CGRectMake(0, 0, self->width,0) inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUnknown animated:YES];
            });
            
            break;
        }
        default:
            break;
    }
}

- (void) alertView:(UIAlertView*) alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (alertView == self->mulliganAlert) {
        if (buttonIndex==1 && self->player->hand->size > 0) {
            apply_mulligan(self->player);
            mulligan();
        } else {
            confirmButton.enabled = true;
            startGame();
        }
    } else if (alertView == self->winnerAlert) {
        attackButton.enabled=false;
        confirmButton.enabled=false;
        endturnButton.enabled=false;
        endGame();
        self->player = newGame(deck_index);
    }
}

- (void) stepperChanged:(UIStepper*)theStepper{
    //NSLog(@"%ld %f",(long)theStepper.tag, theStepper.value);
    int* mana = self->manaBuffer;
    int index = theStepper.tag % 10;
    long old_value = theStepper.tag / 10;
    if (theStepper.value > old_value && mana[index] > 0) { //increment
        mana[index]--;
        mana[0]--;
        paidMana++;
    } else  if (theStepper.value < old_value && mana[index] < player->mana[index]) { //decrement
        mana[index]++;
        mana[0]++;
        paidMana--;
    }

    theStepper.tag = theStepper.value * 10 + index;
    viewController->manaLabel.text = [NSString stringWithFormat:@"Select mana\n(%d/%d)\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d",viewController->paidMana,viewController->pendingMana,mana[1],mana[2],mana[3],mana[4],mana[5]];
    if (paidMana >= pendingMana)
        manaButton.enabled = true;
    else
        manaButton.enabled = false;
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
    [self.deckController freeImages];
    [self.deckController freeStockImages];
}

@end

void displayHand(List* cards) {
    
    [viewController->images removeAllObjects];
    for (UIImageView* iv in viewController->views) {
        [iv removeFromSuperview];
    }
    [viewController->views removeAllObjects];
    viewController->scrollView.layer.borderWidth=0;
    
    int numCards = cards->size > viewController->maxColumns ? viewController->maxColumns : cards->size;
    viewController->scrollView.contentSize = CGSizeMake((viewController->cardWidth+viewController->margin)*numCards,viewController->cardHeight);
    
    NSString* extension = @".jpg";
    for (unsigned int i=0;i<numCards;i++) {
        MTGCard* card = cards->entries[i];
        const char* tag = card->name;
        [viewController->views addObject:[[UIImageView alloc] initWithFrame:CGRectMake((viewController->margin+viewController->cardWidth)*i,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        //[viewController->images addObject:[UIImage imageNamed:fileName]];
        [viewController->images addObject:[viewController loadImage:fileName cached:viewController->cacheImages]];
        [viewController->views[i] setImage: viewController->images[i]];
        [viewController->scrollView addSubview:viewController->views[i]];
    }
    
}

void displayStats(int hp,int librarySize,int handSize,int graveyardSize,int exileSize,int* mana, bool selfOrOpponent) {
    NSString* hpString = [NSString stringWithFormat:
            @"HP: %d\nLibrary: %d\nHand: %d\nGraveyard: %d\nExile: %d\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d\nTotal: %d\n",
            hp,librarySize,handSize,graveyardSize,exileSize,mana[1],mana[2],mana[3],mana[4],mana[5],mana[0]];
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
    currentScrollView.layer.borderWidth=0;
    
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
            //UIImage * portrait = [UIImage imageNamed: fileName];
            UIImage* portrait = [viewController loadImage:fileName cached:viewController->cacheImages];
            UIImage * landscape = [[UIImage alloc] initWithCGImage: portrait.CGImage scale: 1.0 orientation: UIImageOrientationRight];
            [currentImages addObject:landscape];
            x += viewController->cardHeight2 + viewController->margin;
        } else {
            [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,viewController->margin, viewController->cardWidth2, viewController->cardHeight2)]];
            //[currentImages addObject:[UIImage imageNamed:fileName]];
            [currentImages addObject:[viewController loadImage:fileName cached:viewController->cacheImages]];
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
    NSMutableArray* currentLabels;
    if (selfOrOpponent) {
        currentScrollView = viewController->selfBattlefield;
        currentImages = viewController->selfBattlefieldImages;
        currentViews = viewController->selfBattlefieldViews;
        currentLabels = viewController->selfLabels;
    } else {
        currentScrollView = viewController->opponentBattlefield;
        currentImages = viewController->opponentBattlefieldImages;
        currentViews = viewController->opponentBattlefieldViews;
        currentLabels = viewController->opponentLabels;
        viewController->opponentPermanents = permanents;
    }
    
    [currentImages removeAllObjects];
    for (UIImageView* iv in currentViews) {
        [iv removeFromSuperview];
    }
    [currentViews removeAllObjects];
    for (UILabel* lb in currentLabels) {
        [lb removeFromSuperview];
    }
    [currentLabels removeAllObjects];
    currentScrollView.layer.borderWidth=0;
    
    int numCards = permanents->size > viewController->maxColumns ? viewController->maxColumns : permanents->size;
    
    NSString* extension = @".jpg";
    CGFloat x = 0;
    CGFloat tappedMargin = (viewController->gridHeight - viewController->cardWidth)/2;
    for (unsigned int i=0;i<numCards;i++) {
        Permanent* p = permanents->entries[i];
        MTGCard* card = p->source;
        if (card) {
            const char* tag = card->name;
            NSString* currentTag = [NSString stringWithUTF8String:tag];
            NSString* fileName = [currentTag stringByAppendingString:extension];
            if (p->is_tapped) {
                [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,tappedMargin, viewController->cardHeight, viewController->cardWidth)]];
                UIImage* portrait = [viewController loadImage:fileName cached:viewController->cacheImages];
                UIImage * landscape = [[UIImage alloc] initWithCGImage: portrait.CGImage scale: 1.0 orientation: UIImageOrientationRight];
                [currentImages addObject:landscape];
                x += viewController->cardHeight + viewController->margin;
            } else {
                [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
                //[currentImages addObject:[UIImage imageNamed:fileName]];
                [currentImages addObject:[viewController loadImage:fileName cached:viewController->cacheImages]];
                x += viewController->cardWidth + viewController->margin;
            }
            [currentViews.lastObject setImage: currentImages.lastObject];
        } else {
            UIImageView* iv;
            if (p->is_tapped) {
                iv = [[UIImageView alloc] initWithFrame:CGRectMake(x,tappedMargin, viewController->cardHeight, viewController->cardWidth)];
                x += viewController->cardHeight + viewController->margin;
            } else {
                iv = [[UIImageView alloc] initWithFrame:CGRectMake(x,viewController->margin, viewController->cardWidth, viewController->cardHeight)];
                x += viewController->cardWidth + viewController->margin;
            }
            if (p->subtypes.is_white) [iv setBackgroundColor:[UIColor whiteColor]];
            else if (p->subtypes.is_blue) [iv setBackgroundColor:[UIColor blueColor]];
            else if (p->subtypes.is_black)[iv setBackgroundColor:[UIColor blackColor]];
            else if (p->subtypes.is_red) [iv setBackgroundColor:[UIColor redColor]];
            else if (p->subtypes.is_green) [iv setBackgroundColor:[UIColor greenColor]];
            iv.layer.cornerRadius = 5;
            [currentViews addObject:iv];
            [currentImages addObject:[NSNull null]];
        }
        if (p->subtypes.is_planeswalker || p->subtypes.is_creature) {
            UILabel* lb = [[UILabel alloc] init];
            UIImageView* iv = currentViews.lastObject;
            [currentLabels addObject:lb] ;
            if (p->subtypes.is_planeswalker)
                lb.text = [NSString stringWithFormat:@"%d",p->loyalty];
            else if (p->subtypes.is_creature)
                lb.text = [NSString stringWithFormat:@"%d/%d",p->power,p->toughness];
            lb.textColor = [UIColor whiteColor];
            lb.backgroundColor = [UIColor blackColor];
            lb.textAlignment = NSTextAlignmentCenter;
            lb.frame = CGRectMake(iv.frame.size.width-viewController->labelWidth, iv.frame.size.height-viewController->labelHeight, viewController->labelWidth, viewController->labelHeight);
            lb.layer.cornerRadius = 10;
            lb.layer.masksToBounds = YES;
            [iv addSubview:lb];
        }
        [currentScrollView addSubview:currentViews.lastObject];
        
        if (p->equipment) {
            for (unsigned int j=0;j<p->equipment->size;j++) {
                Permanent* q = p->equipment->entries[j];
                NSString* fileName = [[NSString stringWithUTF8String:q->name] stringByAppendingString:@".jpg"];
                [currentViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake(x-viewController->margin,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
                [currentImages addObject:[viewController loadImage:fileName cached:viewController->cacheImages]];
                x += viewController->cardWidth;
                [currentViews.lastObject setImage: currentImages.lastObject];
                [currentScrollView addSubview:currentViews.lastObject];
            }
        }
    }
    
    currentScrollView.contentSize = CGSizeMake(x,viewController->cardHeight);
}

void displayStack(List* stack) {
    [viewController->stackImages removeAllObjects];
    for (UIImageView* iv in viewController->stackViews) {
        [iv removeFromSuperview];
    }
    [viewController->stackViews removeAllObjects];
    viewController->stack.layer.borderWidth=0;
    
    int numCards = stack->size > viewController->maxColumns ? viewController->maxColumns : stack->size;
    viewController->scrollView.contentSize = CGSizeMake((viewController->cardWidth+viewController->margin)*numCards,viewController->cardHeight);
    
    for (unsigned int i=0;i<numCards;i++) {
        Permanent* p = stack->entries[i];
        MTGCard* card = p->source;
        const char* tag = card->name;
        [viewController->stackViews addObject:[[UIImageView alloc] initWithFrame:CGRectMake((viewController->margin+viewController->cardWidth)*i,viewController->margin, viewController->cardWidth, viewController->cardHeight)]];
        NSString* currentTag = [NSString stringWithUTF8String:tag];
        NSString* fileName = [currentTag stringByAppendingString:@".jpg"];

        [viewController->stackImages addObject:[viewController loadImage:fileName cached:viewController->cacheImages]];
        [viewController->stackViews[i] setImage: viewController->stackImages[i]];
        [viewController->stack addSubview:viewController->stackViews[i]];
    }
}

void displayWinner(MTGPlayer* winner) {
    if (winner==viewController->player)
        [viewController->winnerAlert setMessage:@"You won the game!"];
    else
        [viewController->winnerAlert setMessage:@"Opponent won the game!"];
    [viewController->winnerAlert show];
}

void selectMana(int* mana,int amount) {
    viewController->pendingMana = amount;
    viewController->paidMana = 0;
    viewController->manaButton.enabled = amount < 0;
    memcpy(viewController->manaBuffer, viewController->player->mana, 6*sizeof(int));
    viewController->manaLabel.text = [NSString stringWithFormat:@"Select mana\n(%d/%d)\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d",viewController->paidMana,viewController->pendingMana,mana[1],mana[2],mana[3],mana[4],mana[5] ];
    viewController->mode = MANA;
    [viewController.view addSubview:viewController->popupMask];
    [viewController.view addSubview:viewController->manaAlert];
}

void selectBlockers(List* permanentList,List* blockersList) {
    viewController->attackerList = permanentList;
    viewController->blockersList = blockersList;
    viewController->block_index = 0;
    viewController->mode = BLOCK;
    
    int numBlockers = 0;
    for (unsigned int i=0;i<viewController->player->battlefield->size;i++) {
        Permanent* p = viewController->player->battlefield->entries[i];
        if (p->subtypes.is_creature && ! p->is_tapped)
            numBlockers++;
    }
    if (numBlockers > 0) {
        [viewController toggleHighlight:viewController->selfBattlefield];
        Permanent* p = viewController->attackerList->entries[viewController->block_index];
        int i;
        findTarget(p, &i);
        UIImageView* iv = viewController->opponentBattlefieldViews[i];
        [viewController toggleCard:iv];
    }
}

void selectTarget(Permanent* source,char* allowedTargets) {
    [viewController changeMode:STACK];
    void (^blockcommand)(void);
    blockcommand = ^{
        viewController->target_index = allowedTargets[0]=='3' ? 3 : allowedTargets[0]=='2' ? 2 : 0;
        [viewController displayToastWithMessage:[NSString stringWithFormat:@"%s: select target %s",source->name,allowedTargets]];
        [viewController toggleHighlight:viewController->selfBattlefield];
        [viewController toggleHighlight:viewController->opponentBattlefield];
        if (strstr(allowedTargets,"land")) {
            [viewController toggleHighlight:viewController->selfLands];
            [viewController toggleHighlight:viewController->opponentLands];
        }
        if (strstr(allowedTargets,"player")) {
            [viewController toggleCard:viewController->selfDeck];
            [viewController toggleCard:viewController->opponentDeck];
        }
        [viewController changeMode:SELECT_TARGET];
        viewController->currentEquipment = source;
    };
    [viewController->commandQueue addObject:blockcommand];
}

void selectPlayer(Permanent* source) {
    [viewController changeMode:STACK];
    void (^blockcommand)(void);
    blockcommand = ^{
        [viewController displayToastWithMessage:[NSString stringWithFormat:@"%s: select target player",source->name]];
        [viewController toggleCard:viewController->selfDeck];
        [viewController toggleCard:viewController->opponentDeck];
        [viewController changeMode:SELECT_TARGET];
        viewController->currentEquipment = source;
    };
    [viewController->commandQueue addObject:blockcommand];
}

void selectAbility(Permanent* permanent) {
    
}

void selectCards(Permanent* source,List* cards,char* allowedTargets) {
    [viewController changeMode:STACK];
    void (^blockcommand)(void);
    blockcommand = ^{
        [viewController displayToastWithMessage:[NSString stringWithFormat:@"%s: select %s",source->name,allowedTargets]];
        displayHand(cards);
        [viewController toggleHighlight:viewController->scrollView];
        [viewController changeMode: SELECT_CARDS];
        viewController->currentEquipment = source;
    };
    [viewController->commandQueue addObject:blockcommand];
}

void selectOption(Permanent* source,List* options) {
    [viewController changeMode:STACK];
    void (^blockcommand)(void);
    blockcommand = ^{
        for (UIView* v in [viewController->selectSheet subviews]) {
            [v removeFromSuperview];
        }
        for (unsigned int i=0;i<options->size;i++) {
            char* str = options->entries[i];
            [viewController->selectSheet addButtonWithTitle:[NSString stringWithUTF8String:str]];
        }
        [viewController->selectSheet setTitle:[NSString stringWithFormat:@"%s: select option",source->name]];
        viewController->currentEquipment = source;
        [viewController.view addSubview:viewController->selectSheet];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC / 4)), dispatch_get_main_queue(), ^{
            [viewController->selectSheet showInView:viewController.view];
        });
    };
    [viewController->commandQueue addObject:blockcommand];
}

void triggerSelect(char* msg) {
    [viewController changeMode:STACK];
    [viewController displayToastWithMessage:[NSString stringWithFormat:@"Opponent selected %s",msg]];
}

void saveDeck(char* name,List* cards){
    NSArray *paths = NSSearchPathForDirectoriesInDomains
    (NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *fileName = [NSString stringWithFormat:@"%@/%s",
                          documentsDirectory,name];
    FILE* fh = fopen([fileName cStringUsingEncoding:NSUTF8StringEncoding],"w");
    for (unsigned int i=0;i<cards->size;i++) {
        MTGCard* card = cards->entries[i];
        fprintf(fh,"%s\n",card->name);
    }
    fclose(fh);
}

void loadDeck(char* name,List* cards) {
    NSArray *paths = NSSearchPathForDirectoriesInDomains
    (NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *fileName = [NSString stringWithFormat:@"%@/%s",
                          documentsDirectory,name];
    FILE* fh = fopen([fileName cStringUsingEncoding:NSUTF8StringEncoding],"w");
    char buffer[128];
    while (fgets(buffer,128,fh)) {
        MTGCard* card = (MTGCard*) HashTable_findVar(cdt,buffer,(int)strlen(buffer) - 1);
        AppendToList(cards,card);
    }
    fclose(fh);
}

void startTurn(MTGPlayer* player) {
    if (player == viewController->player) {
        viewController->canAttack=true;
        [viewController changeMode:NONE];
        [viewController displayToastWithMessage:@"Your turn"];
    } else {
        [viewController displayToastWithMessage:@"Opponent's Turn"];
    }
}

void discardCards(MTGPlayer* player,int num){
    [viewController changeMode:STACK];
    void (^blockcommand)(void);
    blockcommand = ^{
        viewController->pendingDiscard = num;
        sprintf(viewController->buffer,"Discard %d cards\n",num);
        message(viewController->buffer);
        if (viewController->scrollView.layer.borderWidth == 0)
            [viewController toggleHighlight:viewController->scrollView];
        [viewController changeMode:DISCARD];
    };
    [viewController->commandQueue addObject:blockcommand];
}

void mulligan() {
    viewController->mode=WAIT;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC / 2)), dispatch_get_main_queue(), ^{
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
