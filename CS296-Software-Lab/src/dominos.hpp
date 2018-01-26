/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

/* 
 * Base code for CS 296 Software Systems Lab 
 * Department of Computer Science and Engineering, IIT Bombay
 * Instructor: Parag Chaudhuri
 */

#ifndef _DOMINOS_HPP_
#define _DOMINOS_HPP_

#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif


namespace cs296
{
  //! This is the class that sets up the Box2D simulation world
  //! Notice the public inheritance - why do we inherit the base_sim_t class?
  class dominos_t : public base_sim_t
  {
  public:
    dominos_t();
      
    void keyboard(unsigned char key);
    void keyboard_up(unsigned char key);
    void mouse_down(const b2Vec2& p);
    //! creates dominos_t simulation
	static base_sim_t* create()
	{
		return new dominos_t;
	}
	
	b2Body* m_car;						    //!< body of the car
	b2Body* m_back_hinge;					//!< hinge attached to the car
	b2Body* m_backarm1;						//!< back arm attached to the car
	b2Body* m_backarm2;						//!< another back arm attached to the first back arm
	b2Body* m_frontarm1;					//!< front arm attached to the car
	b2Body* m_digger;						//!< digger attached to the front arm
	b2Body* m_bpick;						//!< picker attached to the back arm
	b2Body* m_piston1;						//!< piston attached to the car
	b2Body* m_piston2;						//!< piston part attached to the front arm
	b2Body* test_box1;						//!< a test box
	b2Body* test_box2;						//!< another test box	
	b2Body* test_ball;						//!< a test ball
	b2Body* obs;							//!< a still obstacle
	float32 m_hz;							//!< frequency value for oscillations in wheel
	float32 m_zeta;							//!< damping rato for wheels
	float32 m_speed;						//!< speed of wheel motors
	b2WheelJoint* m_spring1;				//!< wheel joint for the rear wheel
	b2WheelJoint* m_spring2;				//!< wheel joint for the front wheel
	b2RevoluteJoint* m_mjoint1;				//!< revolute joint between car and backsidearm1
	b2RevoluteJoint* m_mjoint2;				//!< revolute joint between backsidearm1 and backsidearm2	
	b2RevoluteJoint* m_mjoint4;				//!< revolute joint between back pickup and the 2nd back arm
	b2RevoluteJoint* m_mjoint5;				//!< revolute joint between car and frontarm
	b2RevoluteJoint* m_mjoint6;				//!< revolute joint between frontarm and digger
	b2PrismaticJoint* m_pistonjoint;		//!< prismatic joint between piston movement between piston1 and piston2
  };
}
  
#endif
