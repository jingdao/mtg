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
    self->labelWidth = cardHeight/2;
    self->labelHeight = labelWidth/2;
    self->textWidth = 100;
    self->textHeight = 200;
    self->buttonWidth = 100;
    self->buttonHeight = 30;
    self->stepperWidth = 100;
    self->stepperHeight = 35;
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
    UILongPressGestureRecognizer *recog12 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->opponentBattlefield addGestureRecognizer:recog12];
    UILongPressGestureRecognizer *recog13 = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longpress:)];
    [self->opponentLands addGestureRecognizer:recog13];
    
    self->mulliganAlert = [[UIAlertView alloc] initWithTitle:nil message:@"Mulligan?" delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Yes",nil];
    self->manaAlert = [[UIView alloc] initWithFrame: CGRectMake((width-stepperWidth*2-margin*2)/2, (height-margin*4-stepperHeight*5)/2, margin*4+stepperWidth*2, margin*4+stepperHeight*5)];
    manaAlert.backgroundColor = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:1.0];
    manaAlert.layer.cornerRadius = 10.0;
    self->manaLabel = [[UITextView alloc] initWithFrame:CGRectMake(margin*2, margin*2, stepperWidth,margin*2+stepperHeight*5)];
    manaLabel.editable = false;
    manaLabel.backgroundColor = manaAlert.backgroundColor;
    [manaLabel setFont: [UIFont systemFontOfSize:15]];
    [manaAlert addSubview:manaLabel];
    for (int i=1;i<=5;i++) {
        UIStepper* stepper = [[UIStepper alloc] initWithFrame:CGRectMake(stepperWidth+margin*2,margin+stepperHeight*(i-1),stepperWidth,stepperHeight)];
        stepper.tag=i;
        [stepper addTarget:self action:@selector(stepperChanged:) forControlEvents:UIControlEventValueChanged];
        [manaAlert addSubview:stepper];
    }
    
    cd = loadCardData();
    loadCardDataTable();
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
    if ([gesture view] == self->scrollView && player->hand->size > 0) {
        idx = p.x / (self->cardWidth + self->margin);
        if (idx >= self->images.count)
            idx = self->images.count - 1;
        [self->popupImage setImage:self->images[idx]];
        [self.view addSubview:self->popupMask];
        [self.view addSubview:self->popupImage];
    } else if ([gesture view] == self->selfLands && selfLandsViews.count > 0) {
        for (idx=0;idx<self->selfLandsViews.count;idx++) {
            UIImageView *imv = self->selfLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfLandsViews.count)
            idx = self->selfLandsViews.count - 1;
        [self->popupImage setImage:self->selfLandsImages[idx]];
        [self.view addSubview:self->popupMask];
        [self.view addSubview:self->popupImage];
    } else if ([gesture view] == self->selfBattlefield && selfBattlefieldViews.count > 0) {
        for (idx=0;idx<self->selfBattlefieldViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        [self->popupImage setImage:self->selfBattlefieldImages[idx]];
        [self.view addSubview:self->popupMask];
        [self.view addSubview:self->popupImage];
    } else if ([gesture view] == self->opponentLands && opponentLandsViews.count > 0) {
        for (idx=0;idx<self->opponentLandsViews.count;idx++) {
            UIImageView *imv = self->opponentLandsViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentLandsViews.count)
            idx = self->opponentLandsViews.count - 1;
        [self->popupImage setImage:self->opponentLandsImages[idx]];
        [self.view addSubview:self->popupMask];
        [self.view addSubview:self->popupImage];
    } else if ([gesture view] == self->opponentBattlefield && opponentBattlefieldViews.count > 0) {
        for (idx=0;idx<self->opponentBattlefieldViews.count;idx++) {
            UIImageView *imv = self->opponentBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->opponentBattlefieldViews.count)
            idx = self->opponentBattlefieldViews.count - 1;
        [self->popupImage setImage:self->opponentBattlefieldImages[idx]];
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
    unsigned long idx;
    if ([gesture view] == self->scrollView && player->hand->size > 0) {
        idx = p.x / (self->cardWidth + self->margin);
        if (idx >= self->images.count)
            idx = self->images.count - 1;
        if (mode==DISCARD) {
            [self toggleCard:self->views[idx]];
        } else if (mode==NONE){
            if (MTGPlayer_playCard(self->player, (int)idx, self->buffer)) {
                displayHand(self->player->hand);
                displayStats(self->player->hp,self->player->library->size, self->player->hand->size, self->player->mana,true);
                displayLands(self->player->lands, true);
                displayBattlefield(self->player->battlefield, true);
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
        if (mode==NONE) {
            self->currentPermanent = self->player->lands->entries[idx];
            if (!self->currentPermanent->is_tapped)
                MTGPlayer_tap(self->player, self->currentPermanent);
            displayLands(self->player->lands, true);
            displayStats(self->player->hp, self->player->library->size, self->player->hand->size, self->player->mana, self);
        }
    } else if ([gesture view] == self->selfBattlefield && selfBattlefieldViews.count > 0) {
        for (idx=0;idx<self->selfBattlefieldViews.count;idx++) {
            UIImageView *imv = self->selfBattlefieldViews[idx];
            if (p.x < imv.frame.origin.x + imv.frame.size.width)
                break;
        }
        if (idx >= self->selfBattlefieldViews.count)
            idx = self->selfBattlefieldViews.count - 1;
        self->currentPermanent = self->player->battlefield->entries[idx];
        if (mode==ATTACK) {
            if (self->currentPermanent->has_summoning_sickness)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s has summoning sickness!",self->currentPermanent->source->name]];
            else if (self->currentPermanent->is_tapped)
                [self displayToastWithMessage:[NSString stringWithFormat:@"%s cannot attack while tapped!",self->currentPermanent->source->name]];
            else {
                self->currentPermanent->has_attacked= !self->currentPermanent->has_attacked;
                [self toggleCard:selfBattlefieldViews[idx]];
            }
        } else if (mode==NONE) {
            if (!self->currentPermanent->is_tapped)
                MTGPlayer_tap(self->player, self->currentPermanent);
            displayBattlefield(self->player->battlefield, true);
        }
    }
}

- (void) onEndturn: (id)sender{
    if (player->hand->size > 7)
        discardToSeven(player);
    else {
        attackButton.enabled=false;
        confirmButton.enabled=true;
        endturnButton.enabled=false;
        mode = WAIT;
        newTurn();
    }
}

- (void) onPause:(id)sender {
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"New Game",@"Tap All Lands", @"Attack All",nil];
    [sheet showInView:self.view];
}

- (void) onAttack: (id)sender {
    [self toggleHighlight:selfBattlefield];
    mode=ATTACK;
    attackButton.enabled=false;
    confirmButton.enabled=true;
    endturnButton.enabled=false;
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
        displayHand(player->hand);
    } else if (mode == ATTACK) {
        List* permanentList = InitList();
        for (unsigned int i=0;i<player->battlefield->size;i++) {
            Permanent* p = player->battlefield->entries[i];
            if (p->has_attacked) {
                p->is_tapped = true;
                AppendToList(permanentList, p);
            }
        }
        if (permanentList->size > 0)
            resolveAttack(self->player, permanentList);
        else {
            attackButton.enabled=true;
            displayBattlefield(player->battlefield, self);
        }
        DeleteList(permanentList);
    } else if (mode == WAIT) {
        AI_getAction();
        return;
    }
    confirmButton.enabled=false;
    endturnButton.enabled=true;
    mode=NONE;
}

- (void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    switch (buttonIndex) {
        case 0:
            attackButton.enabled=false;
            confirmButton.enabled=false;
            endturnButton.enabled=false;
            endGame();
            self->player = newGame();
            break;
        case 1:
            if (mode == NONE) {
                for (unsigned int i=0;i<self->player->lands->size;i++) {
                    Permanent* p = self->player->lands->entries[i];
                    if (!p->is_tapped) {
                        MTGPlayer_tap(self->player, p);
                    }
                }
                displayLands(self->player->lands, true);
                displayStats(self->player->hp, self->player->library->size, self->player->hand->size, self->player->mana, true);
            }
            break;
        case 2:
            if (mode == NONE && attackButton.enabled==true) {
                [self toggleHighlight:selfBattlefield];
                mode=ATTACK;
                attackButton.enabled=false;
                confirmButton.enabled=true;
                endturnButton.enabled=false;
                for (int i=0;i<self->selfBattlefieldViews.count;i++) {
                    UIImageView* iv = self->selfBattlefieldViews[i];
                    Permanent* p = self->player->battlefield->entries[i];
                    if (!p->has_summoning_sickness && !p->is_tapped) {
                        [self toggleCard:iv];
                        p->has_attacked = true;
                    }
                }
            }
            break;
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
        pendingMana--;
    } else  if (theStepper.value < old_value && mana[index] < player->mana[index]) { //decrement
        mana[index]++;
        mana[0]++;
        pendingMana++;
    }
    if (pendingMana == 0) {
        memcpy(viewController->player->mana, mana, 6*sizeof(int));
        displayStats(player->hp,player->library->size,player->hand->size,player->mana,true);
        self->mode = NONE;
        [self->manaAlert removeFromSuperview];
        [self->popupMask removeFromSuperview];
    } else {
        theStepper.tag = theStepper.value * 10 + index;
        self->manaLabel.text = [NSString stringWithFormat:@"Select mana\n(%d remaining)\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d",viewController->pendingMana,mana[1],mana[2],mana[3],mana[4],mana[5]];
    }
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
        [viewController->images addObject:[UIImage imageNamed:fileName]];
        [viewController->views[i] setImage: viewController->images[i]];
        [viewController->scrollView addSubview:viewController->views[i]];
    }
    
}

void displayStats(int hp,int librarySize,int handSize,int* mana, bool selfOrOpponent) {
    NSString* hpString = [NSString stringWithFormat:
            @"HP: %d\nLibrary: %d\nHand: %d\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d\nTotal: %d\n",
            hp,librarySize,handSize,mana[1],mana[2],mana[3],mana[4],mana[5],mana[0]];
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
        UILabel* lb = [[UILabel alloc] init];
        UIImageView* iv = currentViews[i];
        [currentLabels addObject:lb] ;
        if (p->source->is_planeswalker)
            lb.text = [NSString stringWithFormat:@"%d",p->loyalty];
        else
            lb.text = [NSString stringWithFormat:@"%d/%d",p->power,p->toughness];
        lb.textColor = [UIColor whiteColor];
        lb.backgroundColor = [UIColor blackColor];
        lb.textAlignment = NSTextAlignmentCenter;
        lb.frame = CGRectMake(iv.frame.size.width-viewController->labelWidth, iv.frame.size.height-viewController->labelHeight, viewController->labelWidth, viewController->labelHeight);
        lb.layer.cornerRadius = 10;
        lb.layer.masksToBounds = YES;
        [iv addSubview:lb];
        [currentViews[i] setImage: currentImages[i]];
        [currentScrollView addSubview:currentViews[i]];
    }
}

void selectMana(int* mana,int amount) {
    viewController->pendingMana = amount;
    memcpy(viewController->manaBuffer, viewController->player->mana, 6*sizeof(int));
    viewController->manaLabel.text = [NSString stringWithFormat:@"Select mana\n(%d remaining)\nW: %d\nU: %d\nB: %d\nR: %d\nG: %d",viewController->pendingMana,mana[1],mana[2],mana[3],mana[4],mana[5]];
    viewController->mode = MANA;
    [viewController.view addSubview:viewController->popupMask];
    [viewController.view addSubview:viewController->manaAlert];
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
        viewController->attackButton.enabled=true;
        viewController->confirmButton.enabled=false;
        viewController->endturnButton.enabled=true;
        viewController->mode=NONE;
        [viewController displayToastWithMessage:@"Your turn"];
    } else {
        [viewController displayToastWithMessage:@"Opponent's Turn"];
    }
}

void discardToSeven(MTGPlayer* player){
    sprintf(viewController->buffer,"Please discard down to 7 cards\n");
    message(viewController->buffer);
    if (viewController->scrollView.layer.borderWidth == 0)
        [viewController toggleHighlight:viewController->scrollView];
    viewController->confirmButton.enabled=true;
    viewController->endturnButton.enabled=false;
    viewController->attackButton.enabled=false;
    viewController->mode=DISCARD;
}

void mulligan() {
    viewController->mode=WAIT;
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
