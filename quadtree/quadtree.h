//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_QUADTREE_H
#define MYGAME_64_QUADTREE_H

#include "includes.h"
#include "terrain.h"

static INT8 edgeTable[256][12] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 1, 8, 1, 9,-1,-1,-1,-1,-1,-1},
	{10, 1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0, 1, 2,10,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 2, 9, 2,10,-1,-1,-1,-1,-1,-1},
	{ 3, 2, 8, 2,10, 8, 8,10, 9,-1,-1,-1},
	{11, 2, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 2, 0,11, 0, 8,-1,-1,-1,-1,-1,-1},
	{11, 2, 3, 0, 1, 9,-1,-1,-1,-1,-1,-1},
	{ 2, 1,11, 1, 9,11,11, 9, 8,-1,-1,-1},
	{10, 1, 3,10, 3,11,-1,-1,-1,-1,-1,-1},
	{ 1, 0,10, 0, 8,10,10, 8,11,-1,-1,-1},
	{ 0, 3, 9, 3,11, 9, 9,11,10,-1,-1,-1},
	{ 8,10, 9, 8,11,10,-1,-1,-1,-1,-1,-1},
	{ 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 4, 3, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 1, 9, 0, 8, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 9, 4, 1, 4, 7, 1, 1, 7, 3,-1,-1,-1},
	{10, 1, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 2,10, 1, 0, 4, 7, 0, 7, 3,-1,-1,-1},
	{ 4, 7, 8, 0, 2,10, 0,10, 9,-1,-1,-1},
	{ 2, 7, 3, 2, 9, 7, 7, 9, 4, 2,10, 9},
	{ 2, 3,11, 7, 8, 4,-1,-1,-1,-1,-1,-1},
	{ 7,11, 4,11, 2, 4, 4, 2, 0,-1,-1,-1},
	{ 3,11, 2, 4, 7, 8, 9, 0, 1,-1,-1,-1},
	{ 2, 7,11, 2, 1, 7, 1, 4, 7, 1, 9, 4},
	{ 8, 4, 7,11,10, 1,11, 1, 3,-1,-1,-1},
	{11, 4, 7, 1, 4,11, 1,11,10, 1, 0, 4},
	{ 3, 8, 0, 7,11, 4,11, 9, 4,11,10, 9},
	{ 7,11, 4, 4,11, 9,11,10, 9,-1,-1,-1},
	{ 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 8, 4, 9, 5,-1,-1,-1,-1,-1,-1},
	{ 5, 4, 0, 5, 0, 1,-1,-1,-1,-1,-1,-1},
	{ 4, 8, 5, 8, 3, 5, 5, 3, 1,-1,-1,-1},
	{ 2,10, 1, 9, 5, 4,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3, 5, 4, 9,10, 1, 2,-1,-1,-1},
	{10, 5, 2, 5, 4, 2, 2, 4, 0,-1,-1,-1},
	{ 3, 4, 8, 3, 2, 4, 2, 5, 4, 2,10, 5},
	{11, 2, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 4, 8,11, 2, 8, 2, 0,-1,-1,-1},
	{ 3,11, 2, 1, 5, 4, 1, 4, 0,-1,-1,-1},
	{ 8, 5, 4, 2, 5, 8, 2, 8,11, 2, 1, 5},
	{ 5, 4, 9, 1, 3,11, 1,11,10,-1,-1,-1},
	{ 0, 9, 1, 4, 8, 5, 8,10, 5, 8,11,10},
	{ 3, 4, 0, 3,10, 4, 4,10, 5, 3,11,10},
	{ 4, 8, 5, 5, 8,10, 8,11,10,-1,-1,-1},
	{ 9, 5, 7, 9, 7, 8,-1,-1,-1,-1,-1,-1},
	{ 0, 9, 3, 9, 5, 3, 3, 5, 7,-1,-1,-1},
	{ 8, 0, 7, 0, 1, 7, 7, 1, 5,-1,-1,-1},
	{ 1, 7, 3, 1, 5, 7,-1,-1,-1,-1,-1,-1},
	{ 1, 2,10, 5, 7, 8, 5, 8, 9,-1,-1,-1},
	{ 9, 1, 0,10, 5, 2, 5, 3, 2, 5, 7, 3},
	{ 5, 2,10, 8, 2, 5, 8, 5, 7, 8, 0, 2},
	{10, 5, 2, 2, 5, 3, 5, 7, 3,-1,-1,-1},
	{11, 2, 3, 8, 9, 5, 8, 5, 7,-1,-1,-1},
	{ 9, 2, 0, 9, 7, 2, 2, 7,11, 9, 5, 7},
	{ 0, 3, 8, 2, 1,11, 1, 7,11, 1, 5, 7},
	{ 2, 1,11,11, 1, 7, 1, 5, 7,-1,-1,-1},
	{ 3, 9, 1, 3, 8, 9, 7,11,10, 7,10, 5},
	{ 9, 1, 0,10, 7,11,10, 5, 7,-1,-1,-1},
	{ 3, 8, 0, 7,10, 5, 7,11,10,-1,-1,-1},
	{11, 5, 7,11,10, 5,-1,-1,-1,-1,-1,-1},
	{10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0,10, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 0, 1, 9, 5,10, 6,-1,-1,-1,-1,-1,-1},
	{10, 6, 5, 9, 8, 3, 9, 3, 1,-1,-1,-1},
	{ 1, 2, 6, 1, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3, 2, 6, 5, 2, 5, 1,-1,-1,-1},
	{ 5, 9, 6, 9, 0, 6, 6, 0, 2,-1,-1,-1},
	{ 9, 6, 5, 3, 6, 9, 3, 9, 8, 3, 2, 6},
	{ 3,11, 2,10, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 6, 5,10, 2, 0, 8, 2, 8,11,-1,-1,-1},
	{ 1, 9, 0, 6, 5,10,11, 2, 3,-1,-1,-1},
	{ 1,10, 2, 5, 9, 6, 9,11, 6, 9, 8,11},
	{11, 6, 3, 6, 5, 3, 3, 5, 1,-1,-1,-1},
	{ 0, 5, 1, 0,11, 5, 5,11, 6, 0, 8,11},
	{ 0, 5, 9, 0, 3, 5, 3, 6, 5, 3,11, 6},
	{ 5, 9, 6, 6, 9,11, 9, 8,11,-1,-1,-1},
	{10, 6, 5, 4, 7, 8,-1,-1,-1,-1,-1,-1},
	{ 5,10, 6, 7, 3, 0, 7, 0, 4,-1,-1,-1},
	{ 5,10, 6, 0, 1, 9, 8, 4, 7,-1,-1,-1},
	{ 4, 5, 9, 6, 7,10, 7, 1,10, 7, 3, 1},
	{ 7, 8, 4, 5, 1, 2, 5, 2, 6,-1,-1,-1},
	{ 4, 1, 0, 4, 5, 1, 6, 7, 3, 6, 3, 2},
	{ 9, 4, 5, 8, 0, 7, 0, 6, 7, 0, 2, 6},
	{ 4, 5, 9, 6, 3, 2, 6, 7, 3,-1,-1,-1},
	{ 7, 8, 4, 2, 3,11,10, 6, 5,-1,-1,-1},
	{11, 6, 7,10, 2, 5, 2, 4, 5, 2, 0, 4},
	{11, 6, 7, 8, 0, 3, 1,10, 2, 9, 4, 5},
	{ 6, 7,11, 1,10, 2, 9, 4, 5,-1,-1,-1},
	{ 6, 7,11, 4, 5, 8, 5, 3, 8, 5, 1, 3},
	{ 6, 7,11, 4, 1, 0, 4, 5, 1,-1,-1,-1},
	{ 4, 5, 9, 3, 8, 0,11, 6, 7,-1,-1,-1},
	{ 9, 4, 5, 7,11, 6,-1,-1,-1,-1,-1,-1},
	{10, 6, 4,10, 4, 9,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0, 9,10, 6, 9, 6, 4,-1,-1,-1},
	{ 1,10, 0,10, 6, 0, 0, 6, 4,-1,-1,-1},
	{ 8, 6, 4, 8, 1, 6, 6, 1,10, 8, 3, 1},
	{ 9, 1, 4, 1, 2, 4, 4, 2, 6,-1,-1,-1},
	{ 1, 0, 9, 3, 2, 8, 2, 4, 8, 2, 6, 4},
	{ 2, 4, 0, 2, 6, 4,-1,-1,-1,-1,-1,-1},
	{ 3, 2, 8, 8, 2, 4, 2, 6, 4,-1,-1,-1},
	{ 2, 3,11, 6, 4, 9, 6, 9,10,-1,-1,-1},
	{ 0,10, 2, 0, 9,10, 4, 8,11, 4,11, 6},
	{10, 2, 1,11, 6, 3, 6, 0, 3, 6, 4, 0},
	{10, 2, 1,11, 4, 8,11, 6, 4,-1,-1,-1},
	{ 1, 4, 9,11, 4, 1,11, 1, 3,11, 6, 4},
	{ 0, 9, 1, 4,11, 6, 4, 8,11,-1,-1,-1},
	{11, 6, 3, 3, 6, 0, 6, 4, 0,-1,-1,-1},
	{ 8, 6, 4, 8,11, 6,-1,-1,-1,-1,-1,-1},
	{ 6, 7,10, 7, 8,10,10, 8, 9,-1,-1,-1},
	{ 9, 3, 0, 6, 3, 9, 6, 9,10, 6, 7, 3},
	{ 6, 1,10, 6, 7, 1, 7, 0, 1, 7, 8, 0},
	{ 6, 7,10,10, 7, 1, 7, 3, 1,-1,-1,-1},
	{ 7, 2, 6, 7, 9, 2, 2, 9, 1, 7, 8, 9},
	{ 1, 0, 9, 3, 6, 7, 3, 2, 6,-1,-1,-1},
	{ 8, 0, 7, 7, 0, 6, 0, 2, 6,-1,-1,-1},
	{ 2, 7, 3, 2, 6, 7,-1,-1,-1,-1,-1,-1},
	{ 7,11, 6, 3, 8, 2, 8,10, 2, 8, 9,10},
	{11, 6, 7,10, 0, 9,10, 2, 0,-1,-1,-1},
	{ 2, 1,10, 7,11, 6, 8, 0, 3,-1,-1,-1},
	{ 1,10, 2, 6, 7,11,-1,-1,-1,-1,-1,-1},
	{ 7,11, 6, 3, 9, 1, 3, 8, 9,-1,-1,-1},
	{ 9, 1, 0,11, 6, 7,-1,-1,-1,-1,-1,-1},
	{ 0, 3, 8,11, 6, 7,-1,-1,-1,-1,-1,-1},
	{11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3,11, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 1,11, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 7, 6,11, 3, 1, 9, 3, 9, 8,-1,-1,-1},
	{ 1, 2,10, 6,11, 7,-1,-1,-1,-1,-1,-1},
	{ 2,10, 1, 7, 6,11, 8, 3, 0,-1,-1,-1},
	{11, 7, 6,10, 9, 0,10, 0, 2,-1,-1,-1},
	{ 7, 6,11, 3, 2, 8, 8, 2,10, 8,10, 9},
	{ 2, 3, 7, 2, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 0, 7, 6, 0, 0, 6, 2,-1,-1,-1},
	{ 1, 9, 0, 3, 7, 6, 3, 6, 2,-1,-1,-1},
	{ 7, 6, 2, 7, 2, 9, 2, 1, 9, 7, 9, 8},
	{ 6,10, 7,10, 1, 7, 7, 1, 3,-1,-1,-1},
	{ 6,10, 1, 6, 1, 7, 7, 1, 0, 7, 0, 8},
	{ 9, 0, 3, 6, 9, 3, 6,10, 9, 6, 3, 7},
	{ 6,10, 7, 7,10, 8,10, 9, 8,-1,-1,-1},
	{ 8, 4, 6, 8, 6,11,-1,-1,-1,-1,-1,-1},
	{11, 3, 6, 3, 0, 6, 6, 0, 4,-1,-1,-1},
	{ 0, 1, 9, 4, 6,11, 4,11, 8,-1,-1,-1},
	{ 1, 9, 4,11, 1, 4,11, 3, 1,11, 4, 6},
	{10, 1, 2,11, 8, 4,11, 4, 6,-1,-1,-1},
	{10, 1, 2,11, 3, 6, 6, 3, 0, 6, 0, 4},
	{ 0, 2,10, 0,10, 9, 4,11, 8, 4, 6,11},
	{ 2,11, 3, 6, 9, 4, 6,10, 9,-1,-1,-1},
	{ 3, 8, 2, 8, 4, 2, 2, 4, 6,-1,-1,-1},
	{ 2, 0, 4, 2, 4, 6,-1,-1,-1,-1,-1,-1},
	{ 1, 9, 0, 3, 8, 2, 2, 8, 4, 2, 4, 6},
	{ 9, 4, 1, 1, 4, 2, 4, 6, 2,-1,-1,-1},
	{ 8, 4, 6, 8, 6, 1, 6,10, 1, 8, 1, 3},
	{ 1, 0,10,10, 0, 6, 0, 4, 6,-1,-1,-1},
	{ 8, 0, 3, 9, 6,10, 9, 4, 6,-1,-1,-1},
	{10, 4, 6,10, 9, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 4, 7, 6,11,-1,-1,-1,-1,-1,-1},
	{ 4, 9, 5, 3, 0, 8,11, 7, 6,-1,-1,-1},
	{ 6,11, 7, 4, 0, 1, 4, 1, 5,-1,-1,-1},
	{ 6,11, 7, 4, 8, 5, 5, 8, 3, 5, 3, 1},
	{ 6,11, 7, 1, 2,10, 9, 5, 4,-1,-1,-1},
	{11, 7, 6, 8, 3, 0, 1, 2,10, 9, 5, 4},
	{11, 7, 6,10, 5, 2, 2, 5, 4, 2, 4, 0},
	{ 7, 4, 8, 2,11, 3,10, 5, 6,-1,-1,-1},
	{ 4, 9, 5, 6, 2, 3, 6, 3, 7,-1,-1,-1},
	{ 9, 5, 4, 8, 7, 0, 0, 7, 6, 0, 6, 2},
	{ 4, 0, 1, 4, 1, 5, 6, 3, 7, 6, 2, 3},
	{ 7, 4, 8, 5, 2, 1, 5, 6, 2,-1,-1,-1},
	{ 4, 9, 5, 6,10, 7, 7,10, 1, 7, 1, 3},
	{ 5, 6,10, 0, 9, 1, 8, 7, 4,-1,-1,-1},
	{ 5, 6,10, 7, 0, 3, 7, 4, 0,-1,-1,-1},
	{10, 5, 6, 4, 8, 7,-1,-1,-1,-1,-1,-1},
	{ 5, 6, 9, 6,11, 9, 9,11, 8,-1,-1,-1},
	{ 0, 9, 5, 0, 5, 3, 3, 5, 6, 3, 6,11},
	{ 0, 1, 5, 0, 5,11, 5, 6,11, 0,11, 8},
	{11, 3, 6, 6, 3, 5, 3, 1, 5,-1,-1,-1},
	{ 1, 2,10, 5, 6, 9, 9, 6,11, 9,11, 8},
	{ 1, 0, 9, 6,10, 5,11, 3, 2,-1,-1,-1},
	{ 6,10, 5, 2, 8, 0, 2,11, 8,-1,-1,-1},
	{ 3, 2,11,10, 5, 6,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 6, 3, 9, 6, 3, 8, 9, 3, 6, 2},
	{ 5, 6, 9, 9, 6, 0, 6, 2, 0,-1,-1,-1},
	{ 0, 3, 8, 2, 5, 6, 2, 1, 5,-1,-1,-1},
	{ 1, 6, 2, 1, 5, 6,-1,-1,-1,-1,-1,-1},
	{10, 5, 6, 9, 3, 8, 9, 1, 3,-1,-1,-1},
	{ 0, 9, 1, 5, 6,10,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3,10, 5, 6,-1,-1,-1,-1,-1,-1},
	{10, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 7, 5,11, 5,10,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 8, 7, 5,10, 7,10,11,-1,-1,-1},
	{ 9, 0, 1,10,11, 7,10, 7, 5,-1,-1,-1},
	{ 3, 1, 9, 3, 9, 8, 7,10,11, 7, 5,10},
	{ 2,11, 1,11, 7, 1, 1, 7, 5,-1,-1,-1},
	{ 0, 8, 3, 2,11, 1, 1,11, 7, 1, 7, 5},
	{ 9, 0, 2, 9, 2, 7, 2,11, 7, 9, 7, 5},
	{11, 3, 2, 8, 5, 9, 8, 7, 5,-1,-1,-1},
	{10, 2, 5, 2, 3, 5, 5, 3, 7,-1,-1,-1},
	{ 5,10, 2, 8, 5, 2, 8, 7, 5, 8, 2, 0},
	{ 9, 0, 1,10, 2, 5, 5, 2, 3, 5, 3, 7},
	{ 1,10, 2, 5, 8, 7, 5, 9, 8,-1,-1,-1},
	{ 1, 3, 7, 1, 7, 5,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 0, 0, 7, 1, 7, 5, 1,-1,-1,-1},
	{ 0, 3, 9, 9, 3, 5, 3, 7, 5,-1,-1,-1},
	{ 9, 7, 5, 9, 8, 7,-1,-1,-1,-1,-1,-1},
	{ 4, 5, 8, 5,10, 8, 8,10,11,-1,-1,-1},
	{ 3, 0, 4, 3, 4,10, 4, 5,10, 3,10,11},
	{ 0, 1, 9, 4, 5, 8, 8, 5,10, 8,10,11},
	{ 5, 9, 4, 1,11, 3, 1,10,11,-1,-1,-1},
	{ 8, 4, 5, 2, 8, 5, 2,11, 8, 2, 5, 1},
	{ 3, 2,11, 1, 4, 5, 1, 0, 4,-1,-1,-1},
	{ 9, 4, 5, 8, 2,11, 8, 0, 2,-1,-1,-1},
	{11, 3, 2, 9, 4, 5,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 4, 3, 4, 2, 2, 4, 5, 2, 5,10},
	{10, 2, 5, 5, 2, 4, 2, 0, 4,-1,-1,-1},
	{ 0, 3, 8, 5, 9, 4,10, 2, 1,-1,-1,-1},
	{ 2, 1,10, 9, 4, 5,-1,-1,-1,-1,-1,-1},
	{ 4, 5, 8, 8, 5, 3, 5, 1, 3,-1,-1,-1},
	{ 5, 0, 4, 5, 1, 0,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 0, 4, 5, 9,-1,-1,-1,-1,-1,-1},
	{ 9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 7, 4,11, 4, 9,11,11, 9,10,-1,-1,-1},
	{ 3, 0, 8, 7, 4,11,11, 4, 9,11, 9,10},
	{11, 7, 4, 1,11, 4, 1,10,11, 1, 4, 0},
	{ 8, 7, 4,11, 1,10,11, 3, 1,-1,-1,-1},
	{ 2,11, 7, 2, 7, 1, 1, 7, 4, 1, 4, 9},
	{ 3, 2,11, 4, 8, 7, 9, 1, 0,-1,-1,-1},
	{ 7, 4,11,11, 4, 2, 4, 0, 2,-1,-1,-1},
	{ 2,11, 3, 7, 4, 8,-1,-1,-1,-1,-1,-1},
	{ 2, 3, 7, 2, 7, 9, 7, 4, 9, 2, 9,10},
	{ 4, 8, 7, 0,10, 2, 0, 9,10,-1,-1,-1},
	{ 2, 1,10, 0, 7, 4, 0, 3, 7,-1,-1,-1},
	{10, 2, 1, 8, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 1, 4, 4, 1, 7, 1, 3, 7,-1,-1,-1},
	{ 1, 0, 9, 8, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 3, 4, 0, 3, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 9,10, 8,10,11,-1,-1,-1,-1,-1,-1},
	{ 0, 9, 3, 3, 9,11, 9,10,11,-1,-1,-1},
	{ 1,10, 0, 0,10, 8,10,11, 8,-1,-1,-1},
	{10, 3, 1,10,11, 3,-1,-1,-1,-1,-1,-1},
	{ 2,11, 1, 1,11, 9,11, 8, 9,-1,-1,-1},
	{11, 3, 2, 0, 9, 1,-1,-1,-1,-1,-1,-1},
	{11, 0, 2,11, 8, 0,-1,-1,-1,-1,-1,-1},
	{11, 3, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 2, 2, 8,10, 8, 9,10,-1,-1,-1},
	{ 9, 2, 0, 9,10, 2,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3, 1,10, 2,-1,-1,-1,-1,-1,-1},
	{10, 2, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 1, 3, 8, 9, 1,-1,-1,-1,-1,-1,-1},
	{ 9, 1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};


#define N_CHILDREN 8

class Quad {
public:
    Quad() {}
    Quad(double3 pos, double length);
    ~Quad();

    double3 firstCamPos;

    void update();
    void transform();
    void drawTerrain(bool drawClose);
	void addQuads(int *added, bool onlyVisible);
	void removeQuads(int len);

private:
	double3 pos;
	double3 posAway;
	double distance;
	double length;
	bool subdivide;
	bool combine;
	bool draw;
	Quad *child[N_CHILDREN];

    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
    XMMATRIX groundWorld;
    XMFLOAT3 translation;

    PlanetVertex vertices[(chunkLength)*(chunkLength)*(chunkLength)*12];
    DWORD indices[chunkLength*chunkLength*chunkLength * 6 * 3];
	int nVertices = 0;
	int nIndices = chunkLength * chunkLength * chunkLength * 6 * 3;

    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexBufferData;

	void create();
	void setVertexData();
	void setIndexData();
	void makeChildren();
};

inline Quad::Quad(double3 pos, double length) {
    this->pos = pos;
    this->length = length;
    create();
}

inline void Quad::create() {
    for (int i = 0; i < N_CHILDREN; i++)
        child[i] = nullptr;

    draw = true;
    subdivide = false;
    combine = false;

	setVertexData();
	setIndexData();
}

bool isSame(TerrainPoint a, TerrainPoint b) {
	return (a.noiseHeight < 0.0 && b.noiseHeight < 0.0) || (a.noiseHeight >= 0.0 && b.noiseHeight >= 0.0);
}

TerrainPoint vertInter(TerrainPoint a, TerrainPoint b, double threshold) {
	// return TerrainPoint(a.pos + (b.pos - a.pos) * (threshold - a.noiseHeight) / (b.noiseHeight - a.noiseHeight), threshold);
	return TerrainPoint((a.pos + b.pos) / 2, threshold);
}

inline void Quad::setVertexData() {
	TerrainPoint terrainPoints[(chunkLength + 1)][(chunkLength + 1)][(chunkLength + 1)];

	for (int y = 0; y < (chunkLength + 1); y++) {
		for (int z = 0; z < (chunkLength + 1); z++) {
			for (int x = 0; x < (chunkLength + 1); x++) {
				double3 temp = double3((x - chunkLength / 2.0) / (chunkLength / 2.0) * length, (y - chunkLength / 2.0) / (chunkLength / 2.0) * length, (z - chunkLength / 2.0) / (chunkLength / 2.0) * length);				
				terrainPoints[y][z][x] = TerrainPoint(temp + pos);
			}
		}
	}

	counter = 0;
	for (int z = 0; z < chunkLength; z++) {
		for (int y = 0; y < chunkLength; y++) {
			for (int x = 0; x < chunkLength; x++) {
				TerrainPoint t[8];

				double3 cubeNormal = double3();
				double3 cubeNormals[8] = {
					double3(-1, -1, 1),
					double3(1, -1, 1),
					double3(1, -1, -1),
					double3(-1, -1, 1),
					double3(-1, 1, 1),
					double3(1, 1, 1),
					double3(1, 1, -1),
					double3(-1, 1, -1),
				};

				t[3] = terrainPoints[y][z][x];
				t[2] = terrainPoints[y][z][x + 1];
				t[1] = terrainPoints[y][z + 1][x + 1];
				t[0] = terrainPoints[y][z + 1][x];
				t[7] = terrainPoints[y + 1][z][x];
				t[6] = terrainPoints[y + 1][z][x + 1];
				t[5] = terrainPoints[y + 1][z + 1][x + 1];
				t[4] = terrainPoints[y + 1][z + 1][x];

				bool above = false;
				bool below = false;
				bool belows[8];

				for (int i = 0; i < 8; i++) {
					cubeNormal = cubeNormal + cubeNormals[i] * t[i].noiseHeight;

					belows[i] = false;
					if (t[i].noiseHeight < 0.0) {
						below = true;
						belows[i] = true;
					}
					else
						above = true;
				}

				cubeNormal = normalize(cubeNormal);

				if (above && below) {
					BYTE cubeIndex = 0;
					BYTE bit = 1;
					for (int i = 0; i < 8; i++) {
						if (belows[i]) cubeIndex += bit;
						bit *= 2;
					}

					INT8* edges = edgeTable[cubeIndex];

					for (int i = 0; i < 12; i++) {
						TerrainPoint tp;

						if (edges[i] == -1) break;
						if (edges[i] == 0) {
							tp = vertInter(t[0], t[1], 0.0);
						}
						else if (edges[i] == 1) {
							tp = vertInter(t[1], t[2], 0.0);
						}
						else if (edges[i] == 2) {
							tp = vertInter(t[2], t[3], 0.0);
						}
						else if (edges[i] == 3) {
							tp = vertInter(t[3], t[0], 0.0);
						}
						else if (edges[i] == 4) {
							tp = vertInter(t[4], t[5], 0.0);
						}
						else if (edges[i] == 5) {
							tp = vertInter(t[5], t[6], 0.0);
						}
						else if (edges[i] == 6) {
							tp = vertInter(t[6], t[7], 0.0);
						}
						else if (edges[i] == 7) {
							tp = vertInter(t[7], t[4], 0.0);
						}
						else if (edges[i] == 8) {
							tp = vertInter(t[0], t[4], 0.0);
						}
						else if (edges[i] == 9) {
							tp = vertInter(t[1], t[5], 0.0);
						}
						else if (edges[i] == 10) {
							tp = vertInter(t[2], t[6], 0.0);
						}
						else if (edges[i] == 11) {
							tp = vertInter(t[3], t[7], 0.0);
						}

						double3 temp = tp.pos - firstCamPos;

						vertices[counter].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[counter].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

						if ((i + 1) % 3 == 0) {
							double3 normal = normalize(crossProduct(double3(vertices[counter].pos) - double3(vertices[counter - 1].pos), double3(vertices[counter - 2].pos) - double3(vertices[counter - 1].pos)));
							
							if (dotProduct(normal, cubeNormal) < 0.0)
								normal = normal * -1;
							for (int j = 0; j < 3; j++)
								vertices[counter - j].normal = normal.float3();
						}

						vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));
						vertices[counter].height = tp.noiseHeight;
						vertices[counter++].landTypeHeight = tp.noiseHeight;
					}
					/*
					for (int i = 0; i < 8; i++) {
						double3 temp = t[i].pos - firstCamPos;

						vertices[counter].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[counter].normal = i < 4 ? XMFLOAT3(0.0f, 1.0f, 0.0f) : XMFLOAT3(1.0f, 0.0f, 0.0f);
						vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));

						vertices[counter].height = t[i].noiseHeight;
						vertices[counter++].landTypeHeight = t[i].noiseHeight;
					}*/
				}
			}
		}
	}
	nVertices = counter;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(PlanetVertex) * nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	vertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer);
}

inline void Quad::setIndexData() {
	counter = 0;
	// might need to do order 012032 for gs shader
	for (int i = 0; i < nVertices; i++) {
		// first square
		indices[counter++] = i;
	}
	nIndices = counter;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * nIndices;

	indexBufferData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);
}

inline void Quad::update() {
    posAway = pos - camPos;
    distance = vLength(posAway);

    if (distance < length * 16.0 && length > minLength) {
        subdivide = true;
        combine = false;
    } else if (distance > length * 48.0 && length < maxLength) {
        combine = true;
        subdivide = false;
    } else {
        subdivide = false;
        combine = false;
    }

	if (child[0] == nullptr)
		draw = true;
	else
		draw = false;

	for (int i = 0; i < N_CHILDREN; i++) {
		if (child[i] != nullptr)
			child[i]->update();
	}
}

inline void Quad::addQuads(int *added, bool onlyVisible) {
	if (*added < 1) {
		if (subdivide && child[0] != nullptr) {
			for (int i = 0; i < N_CHILDREN; i++)
				child[i]->addQuads(added, onlyVisible);
		}
		else if (subdivide && (dotProduct(posAway, camDir) > 0.5 || !onlyVisible)) {
			if (nVertices > 0) {
				makeChildren();
				(*added)++;
			}
		}
	}
}

inline void Quad::removeQuads(int len) {
	if (child[0] != nullptr) {
		bool rem = true;
		for (int i = 0; i < N_CHILDREN; i++) {
			if (!child[i]->draw) {
				child[i]->removeQuads(len);
				rem = false;
			}
			else if (!child[i]->combine || child[i]->length != len) {
				rem = false;
			}
		}
		if (rem) {
			for (int i = 0; i < N_CHILDREN; i++) {
				delete child[i];
				child[i] = nullptr;
			}
		}
	}
}

inline void Quad::transform() {
    groundWorld = XMMatrixIdentity();
    Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
    translation = XMFLOAT3(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
    groundWorld = Scale * Translation;

	for (int i = 0; i < N_CHILDREN; i++) {
		if (child[i] != nullptr)
			child[i]->transform();
	}
}

inline void Quad::drawTerrain(bool drawClose) {
    if (draw) {
        if (dotProduct(posAway, camDir) > 0.5 || distance < length * 2.0) {
            if (distance < 900 && drawClose || distance >= 900 && !drawClose) {
                if (drawClose) {
                    if (length == minLength)
                        // d3d11DevCon->GSSetShader(PLANET_GS, 0, 0);
						d3d11DevCon->GSSetShader(NULL, 0, 0);
                    else
                        d3d11DevCon->GSSetShader(NULL, 0, 0);
                }

                UINT stride = sizeof(PlanetVertex);
                UINT offset = 0;

                d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);

                //Set the WVP matrix and send it to the constant buffer in effect file
                //WVP = groundWorld * camView * camProjection;
                if (distance < 900.0f)
                    WVP = groundWorld * camView * camProjection[0];
                else
                    WVP = groundWorld * camView * camProjection[1];

                //cbPerPlanetObj.WVP = XMMatrixTranspose(WVP);
                //cbPerPlanetObj.World = XMMatrixTranspose(groundWorld);
                cbPerPlanetObj.Translation = translation;
                cbPerPlanetObj.View = XMMatrixTranspose(camView);

                if (distance < 900.0f)
                    cbPerPlanetObj.Proj = XMMatrixTranspose(camProjection[0]);
                else
                    cbPerPlanetObj.Proj = XMMatrixTranspose(camProjection[1]);

                d3d11DevCon->UpdateSubresource(cbPerPlanetBuffer, 0, NULL, &cbPerPlanetObj, 0, 0);
                d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerPlanetBuffer);
                d3d11DevCon->GSSetConstantBuffers(0, 1, &cbPerPlanetBuffer);
                d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
                d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
                d3d11DevCon->PSSetShaderResources(2, 1, &texture[6]);
                d3d11DevCon->PSSetShaderResources(3, 1, &texture[5]);
                d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

				/*if (distance < 900.0f)
					d3d11DevCon->RSSetState(RSCullNone); // For grass
				else
					d3d11DevCon->RSSetState(CCWcullMode);*/
				d3d11DevCon->RSSetState(RSCullNone);
                //d3d11DevCon->RSSetState(Wireframe);
                d3d11DevCon->DrawIndexed(nIndices, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < N_CHILDREN; i++) {
			if (child[i] != nullptr)
				child[i]->drawTerrain(drawClose);
        }
    }
}

inline void Quad::makeChildren() {
    for (int i = 0; i < N_CHILDREN; i++) {
        child[i] = new Quad();

        if (i == 0) {
            child[i]->pos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 1) {
            child[i]->pos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 2) {
            child[i]->pos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 3) {
            child[i]->pos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 4) {
			child[i]->pos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 5) {
			child[i]->pos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 6) {
			child[i]->pos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		} else if (i == 7) {
			child[i]->pos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		}
        
        child[i]->length = length / 2.0;
        child[i]->firstCamPos = camPos;

        child[i]->subdivide = false;
        child[i]->combine = false;
        child[i]->draw = true;

        child[i]->create();
    }
}

inline Quad::~Quad() {
	if (vertBuffer != nullptr)
		vertBuffer->Release();
	if (indexBuffer != nullptr)
		indexBuffer->Release();
    for (int i = 0; i < N_CHILDREN; i++) {
        delete child[i];
        child[i] = nullptr;
    }
}



#endif MYGAME_64_QUADTREE_H
