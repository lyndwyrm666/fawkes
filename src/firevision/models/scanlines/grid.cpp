
/***************************************************************************
 *  grid.cpp - Implementation of the grid scanline model
 *
 *  Generated: Tue Feb 22 10:36:39 2005
 *  Copyright  2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#include <models/scanlines/grid.h>
#include <core/exceptions/software.h>

#include <cstring>

using fawkes::point_t;

/** @class ScanlineGrid <models/scanlines/grid.h>
 * Scanline Grid.
 * A grid as scanline points. The crossings of the lines are the scanline
 * points.
 */

/** Constructor.
 * @param width width of grid
 * @param height height of grid
 * @param offset_x x offset between lines
 * @param offset_y y offset between lines
 * @param roi the grid will only be calculated within the roi (if NULL the roi 
 *            will be from 0,0 to width,height). The object will be deleted by 
 *            ScanlineGrid!
 * @param horizontal_grid if true x will be increased before y
 */
ScanlineGrid::ScanlineGrid(unsigned int width, unsigned int height,
			   unsigned int offset_x, unsigned int offset_y, 
			   ROI* roi, bool horizontal_grid)
{
  this->roi = NULL;
  setGridParams(width, height, 
                offset_x, offset_y, 
                roi, horizontal_grid);
  //reset is done in setGridParams ()
}

/** Destructor
 */
ScanlineGrid::~ScanlineGrid()
{
  if (roi) //Has to be set, but still...
  {
    delete roi;
  }
}

point_t
ScanlineGrid::operator*()
{
  return coord;
}

point_t*
ScanlineGrid::operator->()
{
  return &coord;
}

void 
ScanlineGrid::calc_next_coord()
{
  if (finished())
    return;
  
  if (horizontal_grid)
  {
    if (coord.x < (roi->image_width - offset_x))
    {
      coord.x += offset_x;
    } 
    else
    {
      if (coord.y < (roi->image_height - offset_y))
      {
        coord.x = roi->start.x;
        coord.y += offset_y;
      }
      else
      {
        more_to_come = false;
      }
    }
  } 
  else // vertical grid
  {
    if (coord.y < (roi->image_height - offset_y))
    {
      coord.y += offset_y;
    } 
    else
    {
      if (coord.x < (roi->image_width - offset_x))
      {
        coord.x += offset_x;
        coord.y = roi->start.y;
      }
      else
      {
        more_to_come = false;
      }
    }
  }
}

point_t *
ScanlineGrid::operator++()
{
  calc_next_coord();
  return &coord;
}

point_t *
ScanlineGrid::operator++(int)
{
  memcpy(&tmp_coord, &coord, sizeof(point_t));
  calc_next_coord();
  return &tmp_coord;
}

bool
ScanlineGrid::finished()
{
  return !more_to_come;
}

void
ScanlineGrid::reset()
{
  coord.x = roi->start.x;
  coord.y = roi->start.y;

  more_to_come = true;
}

const char *
ScanlineGrid::get_name()
{
  return "ScanlineModel::Grid";
}


unsigned int
ScanlineGrid::get_margin()
{
  return (offset_x > offset_y) ? offset_x : offset_y;
}


void
ScanlineGrid::set_robot_pose(float x, float y, float ori)
{
  // ignored
}


void
ScanlineGrid::set_pan_tilt(float pan, float tilt)
{
  // ignored
}


/** Set dimensions.
 * Set width and height of scanline grid. Implicitly resets the grid.
 * @param width width
 * @param height height
 * @param roi the grid will only be calculated within the roi (if NULL the roi 
 *            will be from 0,0 to width,height). The object will be deleted by 
 *            ScanlineGrid!
 */
void
ScanlineGrid::setDimensions(unsigned int width, unsigned int height, ROI* roi)
{
  this->width  = width;
  this->height = height;

  if (this->roi)
    delete this->roi;
  
  if (!roi)
    this->roi = new ROI(0, 0, this->width, this->height, this->width, this->height);
  else 
  {
    this->roi = roi;
    //Use roi image width/height as grid boundary
    this->roi->set_image_width(this->roi->start.x + this->roi->width);
    this->roi->set_image_height(this->roi->start.y + this->roi->height);
    
    if (this->roi->image_width > this->width)
      throw fawkes::OutOfBoundsException("ScanlineGrid: ROI is out of grid bounds!", this->roi->image_width, 0, this->width);
    if (this->roi->image_height > this->height)
      throw fawkes::OutOfBoundsException("ScanlineGrid: ROI is out of grid bounds!", this->roi->image_height, 0, this->height);
  }
  
  reset();
}


/** Set offset.
 * Set X and Y offset by which the pointer in the grid is advanced. Implicitly resets the grid.
 * @param offset_x offset_x
 * @param offset_y offset_y
 */
void
ScanlineGrid::setOffset(unsigned int offset_x, unsigned int offset_y)
{
  this->offset_x = offset_x;
  this->offset_y = offset_y;

  reset();
}


/** Set all grid parameters.
 * Set width, height, X and Y offset by which the pointer in the grid is advanced.
 * Implicitly resets the grid.
 * @param width width
 * @param height height
 * @param offset_x offset_x
 * @param offset_y offset_y
 * @param roi the grid will only be calculated within the roi (if NULL the roi 
 *            will be from 0,0 to width,height). The object will be deleted by 
 *            ScanlineGrid!
 * @param horizontal_grid if true x will be increased before y
 */
void
ScanlineGrid::setGridParams(unsigned int width, unsigned int height,
                            unsigned int offset_x, unsigned int offset_y, 
                            ROI* roi, bool horizontal_grid)
{
  this->horizontal_grid = horizontal_grid;

  setDimensions(width, height, roi);
  setOffset (offset_x, offset_y);
}
