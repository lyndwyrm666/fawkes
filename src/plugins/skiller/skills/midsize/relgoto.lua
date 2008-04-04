
----------------------------------------------------------------------------
--  relgoto.lua - relative goto
--
--  Created: Tue Mar 25 16:31:42 2008
--  Copyright  2008  Tim Niemueller [www.niemueller.de]
--
--  $Id$
--
----------------------------------------------------------------------------

--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Library General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software Foundation,
--  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

require("midsize")
module("midsize.relgoto", midsize.module_init)

-- constants
local DEFAULT_MARGIN = 0.2;

-- skill state variables
local msgid = 0;

-- Check status of goto.
-- @param margin the radius of a circle around the destination point,
-- if the robot is within that circle the goto is considered final.
function relgoto_checkstatus(margin)
   assert(margin, "Invalid margin supplied");

   print("Relgoto check status");

   if msg == nil then -- we did not yet enqueue a goto message
      return S_RUNNING;
   elseif msgid == navigator:msgid() then
      if dest_dist < margin then
	 return S_FINAL;
      else
	 return S_RUNNING;
      end
   else
      return S_FAILED;
   end
end


-- parameter parsing to support different call styles
function relgoto_parseparams(...)
   local x, y, ori, phi, dist, margin;

   local f = ...; -- first var
   if type(f) == "table" then
      -- it's called with style 2. or 3.
      if f.x ~= nil and f.y ~= nil then
	 -- style 2.
	 x      = f.x;
	 y      = f.y;
	 ori    = f.ori;
	 margin = f.margin;
	 print_debug("2. relgoto(x=" .. x .. ", y=" .. y .. ", ori=" .. tostring(ori) .. ", margin=" .. tostring(margin) .. ")");
      elseif f.phi ~= nil and f.dist ~= nil then
	 -- style 3.
	 phi    = f.phi;
	 dist   = f.dist;
	 ori    = f.ori;
	 margin = f.margin;
	 print_debug("3. relgoto(phi=" .. phi .. ", dist=" .. dist .. ", ori=" .. tostring(ori) .. ", margin=" .. tostring(margin) .. ")");
      else
	 error("relgoto called with insufficient parameters");
      end
   else
      -- positional style
      x, y, ori, margin = ...;
      if x == nil or y == nil then
	 error("Insufficient parameters for relgoto (positional)");
      end
      print_debug("1. relgoto(x=" .. x .. ", y=" .. y .. ", ori=" .. tostring(ori) .. ", margin=" .. tostring(margin) .. ")");
   end

   return x, y, ori, phi, dist, margin;
end


function relgoto_reset()
   msg = nil;
   msgid = 0;
   print("Relgoto reset");
end


function relgoto(...)
   local x, y, ori, phi, dist, margin = relgoto_parseparams(...);

   -- default values for margin and orientation
   if tonumber(margin) == nil then
      margin = DEFAULT_MARGIN;
   end
   if tonumber(ori) == nil then
      ori = 0;
   end

   -- Check if we reached the destination or if we cannot at all
   local status = relgoto_checkstatus(margin);
   if status ~= S_RUNNING then
      return status;
   end

   if navigator:has_writer() then
      if x ~= nil and y ~= nil then
	 -- send CartesianGotoMessage
	 local m = navigator.CartesianGotoMessage:new(x, y, ori);
	 msgid   = navigator:msgq_enqueue_copy(m);
      else
	 -- send PolarGotoMessage
	 local m = navigator.PolarGotoMessage:new(phi, ori, ori);
	 msgid = navigator:msgq_enqueue_copy(m);
      end
   else
      print_error("Navigator not loaded, cannot execute relgoto");
      return S_FAILED;
   end

   return S_RUNNING;
end

relgoto_skill_doc = [==[Relative goto skill.
This skill takes you to a position given in relative coordinates in the robot-local
coordinate system. The orientation is the final orientation, nothing is said about the
intermediate orientation while on the way. The margin is the precision of the relgoto
command. The relgoto is considered final if the current (x,y) is in a radius of the
given margin around the destination. For example is the margin is 0.5 then the relgoto
is considered final if the robot is in a circle of 0.5m radius around the target point.
The default margin is 0.2m.

There are several forms to call this skill:
1. relgoto(phi, dist[, ori[, margin])
   This will goto the position giving in the relative polar coordinates, optionally with
   the given orientation.
2. relgoto{x=X, y=Y[, ori=ORI][, margin=MARGIN]}
   Go to the relative cartesian coordinates (X,Y) with the optional final orientation ORI.
3. relgoto{phi=PHI, dist=DIST[, ori=ORI], [margin=MARGIN]}
   Same as 1., but with named arguments.

Parameters:
phi, dist: robot-relative polar coordinates of target point
x, y:      robot-relative cartesian coordinates of target point
ori:       orientation of robot at destination, radian offset from current value
           clock-wise positive
margin:    radius of a circle around the destination point, if the robot is within
           that circle the goto is considered final.

The skill is S_RUNNING as long as the target can still be reached, S_FINAL if the target
has been reached (at least once, the robot might move afterwards for example if it did not
brake fast enough or if another robot crashed into this robot). The skill is S_FAILED if
the navigator started processing another goto message.
]==]

register_skill{name       = "relgoto",
	       func       = relgoto,
	       doc        = relgoto_skill_doc};
