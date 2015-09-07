//
//  ViewController.m
//  MTG_iOS
//
//  Created by jeewoongpark on 9/7/15.
//  Copyright (c) 2015 jeewoongpark. All rights reserved.
//

#import "ViewController.h"
#include "CardData.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self->coverImage = [UIImage imageNamed:@"cover.jpg"];
    int width = self.view.frame.size.width;
    int height = self.view.frame.size.height;
    int margin = 10;
    int numColumn = 5;
    CGFloat coverWidth = self->coverImage.size.width;
    CGFloat coverHeight = self->coverImage.size.height;
    CGFloat cardWidth = (width - (numColumn + 1)*margin)/numColumn;
    CGFloat cardHeight = cardWidth/coverWidth*coverHeight;
    self->cover = [[UIImageView alloc] initWithFrame:CGRectMake(0,0,cardWidth,cardHeight)];
    [self->cover setImage: self->coverImage];
    [self.view addSubview:self->cover];
    
    CardData cd = loadCardData();
    const char* tag[numColumn];
    tag[0] = cd.AccursedSpirit->name;
    tag[1] = cd.ActonImpulse->name;
    tag[2] = cd.AegisAngel->name;
    tag[3] = cd.AeronautTinkerer->name;
    tag[4] = cd.Aetherspouts->name;
    
    NSString* extension = @".jpg";
    for (int i=0;i<numColumn;i++) {
        CGFloat x = (i+1)*margin+i*cardWidth;
        CGFloat y = height - margin - cardHeight;
        self->views[i] = [[UIImageView alloc] initWithFrame:CGRectMake(x,y, cardWidth, cardHeight)];
        NSString* currentTag = [NSString stringWithUTF8String:tag[i]];
        NSString* fileName = [currentTag stringByAppendingString:extension];
        self->images[i] = [UIImage imageNamed:fileName];
        [self->views[i] setImage: self->images[i]];
        [self.view addSubview:self->views[i]];
    }

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
