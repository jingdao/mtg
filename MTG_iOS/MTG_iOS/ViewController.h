//
//  ViewController.h
//  MTG_iOS
//
//  Created by jeewoongpark on 9/7/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "MTGController.h"

@interface ViewController : UIViewController {
    int width,height,margin,numColumn,maxColumns;
    int popupWidth,popupHeight,textWidth;
    CGFloat cardWidth,cardHeight;
    UIImageView* selfDeck;
    UIImageView* opponentDeck;
    UIImage* coverImage;
    NSMutableArray* views;
    NSMutableArray* images;
    UIScrollView* scrollView;
    UIImageView* popupImage;
    UIView* popupMask;
    @public UITextView* selfHP;
    @public UITextView* opponentHP;
    MTGPlayer* player;
    char buffer[128];
}

@end

