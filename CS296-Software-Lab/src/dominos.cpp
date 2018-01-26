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


#include "cs296_base.hpp"
#include "render.hpp"
#include "callbacks.hpp"
#include <math.h>
#include <time.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include "GL/freeglut.h"
#endif

#include <cstring>
#include <iostream>
using namespace std;

#include "dominos.hpp"

namespace cs296
{
  /**This is the constructor for dominos_t class.
   * All simulating bodies are defined here.
   */ 
  dominos_t::dominos_t()
    {		
		m_hz = 10.0f;
		m_zeta = 0.2f;	
		m_speed = 30.0f; 
		int num=360;
		int count=0;
		float y;
		
		/** This creates a ground body in the simulation world using an edge shape.
		 * 	Here friction is set to 0.6 for the car to move. 
		 */
		b2Vec2 groundVertices[num];
		{
			b2BodyDef bd;
			bd.position.Set(0.0f,0.0f);
			bd.type=b2_staticBody;
			b2Body* ground = m_world->CreateBody(&bd);
			int w=0;
			b2ChainShape groundShape;
			y=-20;
			float y1;
			srand (time(NULL));
			for(int j=0;j<7;j++) {
				for(int i=0;i<45;i++) {
					y1=rand() % 2 - 1;
					if(w==1) groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y - y1);
					else if(w==2) groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y + y1);
					else groundVertices[j*45 +i].Set(3*(j*45.0f +i)-100.0f , y);
					y=groundVertices[j*45 +i].y;	
					count+=1;
				}
				w=rand() % 3  ;
			}
			
			groundShape.CreateChain(groundVertices,count);
			b2FixtureDef fd;
			fd.shape = &groundShape;
			fd.density = 0.0f;
			fd.friction = 0.6f;
			
			ground->CreateFixture(&fd);
		}

		/** This is where the car body is created by creating all it's parts individually 
		 *  and joining them using various joints like weld joint, revolute joint and 
		 * 	distance joints.
		 */
		{
			/** This is where the body of the chassis is defined by using a five-sided polygon.
			 * 	The density is kept 5.0f and the group Index is kept -1 in order not to have collision
			 * 	with the back arms and front arms.
			 * 	the upper rectangular part of the chassis is attached to the lower part by weld joint.
			 * 	A rectanglar hinge is attached to the chassis to support the back arm.
			 */
			{
				//! creates a chassis shape using b2PolygonShape
				b2PolygonShape chassis;
				b2Vec2 vertices[5];
				vertices[0].Set(-19.0f, -05.0f);
				vertices[1].Set(19.0f, -05.0f);
				vertices[2].Set(19.0f, 5.0f);
				vertices[3].Set(0.0f, 8.0f);
				vertices[4].Set(-19.0f, 8.0f);
				chassis.Set(vertices, 5);

				
				//!creates a fixture for the chassis with the polygon shape and 5.0f density.
				b2FixtureDef fdchassis;
				fdchassis.shape = &chassis;
				fdchassis.density = 5.0f;
				fdchassis.filter.groupIndex =-1;
				
				//!creates the chassis body in m_world using the above fixture	
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(0.0f, -10.0f);
				m_car = m_world->CreateBody(&bd);
				m_car->CreateFixture(&fdchassis);
				
				//!upper-part of the chassis is created
				b2PolygonShape chassis_up;
				chassis_up.SetAsBox(9.5,7);
				fdchassis.shape = &chassis_up;
				fdchassis.filter.groupIndex = -1;
				bd.position.Set(-9.5f,5.0f);
				b2Body* m_car_up = m_world->CreateBody(&bd);
				m_car_up->CreateFixture(&fdchassis);
				
				//!upper-part of the chassis is welded to the lower part
				b2WeldJointDef wj;
				wj.Initialize(m_car, m_car_up, m_car_up->GetPosition());
				m_world->CreateJoint(&wj);
				
				//!A hinge is created to support back arms
				b2PolygonShape hinge;
				hinge.SetAsBox(2,2);
				fdchassis.shape = &hinge;
				bd.position.Set(-21.0f,-13.0f);
				m_back_hinge = m_world->CreateBody(&bd);
				m_back_hinge->CreateFixture(&fdchassis);
				
				//!hinge is attached to the chassis
				wj.Initialize(m_car, m_back_hinge, m_back_hinge->GetPosition());
				m_world->CreateJoint(&wj);
			}
			
			/** Silencer Pipe.
			 *  A silencer pipe is created at the top of the chassis.
			 */ 
			{
				//!Pipe shape using b2PolygonShape	
				b2PolygonShape pipe;
				b2Vec2 vertices[5];
				vertices[0].Set(-1.2f, 0.0f);
				vertices[1].Set(1.2f, 0.0f);
				vertices[2].Set(1.0f, 1.0f);
				vertices[3].Set(-1.0f, 1.0f);
				pipe.Set(vertices, 4);
				
				//!Pipe fixture using the pipe shape and 0.5 density
				b2FixtureDef fdpipe;
				fdpipe.shape = &pipe;
				fdpipe.filter.groupIndex = -1;
				fdpipe.density = 0.5f;
				
				//!creates Pipe body in m_world	
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(10.0f, -3.8f);
				b2Body* m_pipe_part1 = m_world->CreateBody(&bd);
				m_pipe_part1->CreateFixture(&fdpipe);
				
				//!welds the Pipe with Pipe Body
				b2WeldJointDef wj;
				wj.Initialize(m_car, m_pipe_part1, m_pipe_part1->GetPosition());
				m_world->CreateJoint(&wj);
				
				//!creates upper part of the  silencer pipe
				vertices[0].Set(1.0f, 1.0f);
				vertices[1].Set(1.0f, 7.0f);
				vertices[2].Set(0.0f, 9.0f);
				vertices[3].Set(-1.0f, 7.0f);
				vertices[4].Set(-1.0f, 1.0f);
				pipe.Set(vertices, 5);
				
				b2Body* m_pipe_part2 = m_world->CreateBody(&bd);
				m_pipe_part2->CreateFixture(&fdpipe);
				
				//!weld both the pipe parts together to get a good feel of silencer
				wj.Initialize(m_pipe_part1, m_pipe_part2, m_pipe_part2->GetPosition());
				m_world->CreateJoint(&wj);
				
				vertices[0].Set(1.0f, 7.0f);
				vertices[1].Set(1.5f, 7.2f);
				vertices[2].Set(1.5f, 9.2f);
				vertices[3].Set(0.0f, 9.0f);
				pipe.Set(vertices, 4);
				
				m_pipe_part2->CreateFixture(&fdpipe);
			}
			
			/** creates wheels for the car.
			 * 	By using a wheel joint as opposed to a revolute joint it ensures proper suspension
			 * 	and nice feel. Damping ratio and frequency is given proporly for the wheel to look natural.
			 *  The friction is kept 0.9f to move well. The motor of the front wheel is disabled so that 
			 * 	the car will travel using only rear wheel.
			 */ 
			{
				//!creates circular shape for wheel
				b2CircleShape circle;
				circle.m_radius = 7.0f;
				
				//!creates fixture for wheel using the circular shape
				b2FixtureDef fd;
				fd.shape = &circle;
				fd.density = 5.0f;
				fd.friction = 0.9f;
				fd.restitution = 0.9f;
				fd.filter.groupIndex=-1;
				
				//!creates rear wheel using fd fixture
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-14.0f, -13.5f);
				b2Body* m_wheel1 = m_world->CreateBody(&bd);
				m_wheel1->CreateFixture(&fd);
				
				//!creates front wheel using fd fixture
				circle.m_radius = 6.0f;
				bd.position.Set(13.0f, -14.0f);
				b2Body* m_wheel2 = m_world->CreateBody(&bd);
				m_wheel2->CreateFixture(&fd);
				
				//!creates a wheel joint prototype
				b2WheelJointDef jd;
				b2Vec2 axis(0.0f, 1.0f);
				
				//!places the joint in the m_world using wheel joint prototype for the rear wheel
				jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
				jd.motorSpeed = 0.0f;
				jd.maxMotorTorque = 150000.0f;
				jd.enableMotor = true;
				jd.frequencyHz = m_hz;
				jd.dampingRatio = m_zeta;
				m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);
				
				//!places the joint in the m_world using wheel joint prototype for the front wheel
				jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
				jd.motorSpeed = 0.0f;
				jd.maxMotorTorque = 150000.0f;
				jd.enableMotor = false;
				jd.frequencyHz = m_hz;
				jd.dampingRatio = m_zeta;
				m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
				
				//m_spring1->SetMotorSpeed(-m_speed); //for testing purpose
			}
			
			/** Backside arm attached to the back_hinge.
			 */ 	
			{	
				//! back side arm's shape is created using a polygon.
				b2PolygonShape backarm1;
				b2Vec2 vertices[5];
				vertices[0].Set(0.0f, -10.0f);
				vertices[1].Set(2.0f, -10.0f);
				vertices[2].Set(2.0f, 0.0f);
				vertices[3].Set(-4.0f, 16.0f);
				vertices[4].Set(-5.0f, 16.0f);
				backarm1.Set(vertices,5);
				
				//!fixture using the above shape
				b2FixtureDef fdbackarm1;
				fdbackarm1.shape = &backarm1;
				fdbackarm1.density = 1.0f;
				fdbackarm1.filter.groupIndex = -1;
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-22.0f, -5.0f);
				m_backarm1 = m_world->CreateBody(&bd);
				m_backarm1->CreateFixture(&fdbackarm1);
			}
			
			/**Backsidearm2 attached to backsidearm.
			 */ 
			{
				//! Back Arm2 shape using b2PolygonShape	
				b2PolygonShape backarm2;
				b2Vec2 vertices[6];
				vertices[0].Set(-0.5f, -14.0f);
				vertices[1].Set(-2.0f, 13.5f);
				vertices[2].Set(-1.33f, 14.0f);
				vertices[3].Set(1.33f, 14.0f);
				vertices[4].Set(2.0f, 13.5f);
				vertices[5].Set(0.5f, -14.0f);
				backarm2.Set(vertices,6);
				b2FixtureDef fdbackarm2;
				fdbackarm2.shape = &backarm2;
				fdbackarm2.density = 1.0f;
				
				//! Back Arm2 body
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-28.0f, -3.0f);
				m_backarm2 = m_world->CreateBody(&bd);
				m_backarm2->CreateFixture(&fdbackarm2);
			}
			/** The joint that joins backside arm 1 with chassis.
			 * 	By changing the motor speed on key press we can control
			 * 	the rotation about this joint.
			 */ 
			{	
				//! Joint using b2RevoluteJointDef
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_back_hinge;
				revoluteJointDef.bodyB = m_backarm1;
				revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(0,0);
				revoluteJointDef.localAnchorB.Set(1,-9);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = -20 * DEGTORAD;
				revoluteJointDef.upperAngle =  30 * DEGTORAD;
				revoluteJointDef.enableMotor = true;
				revoluteJointDef.maxMotorTorque = 30000.0f;
				m_mjoint1 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
				
				//m_mjoint1->SetMotorSpeed(-1.0f) ;//for test purpose
			}
			
			/** The  joint that joins backside arm 1 with backside arm 2.
			 *  By changing the motor speed on key press we can control
			 * 	the rotation about this joint.
			 */  
			{	
				//! Joint definition using b2RevoluteJointDef
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_backarm1;
				revoluteJointDef.bodyB = m_backarm2;
				//revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(-4.5,16);
				revoluteJointDef.localAnchorB.Set(0,13.0);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = -(80 * DEGTORAD);
				revoluteJointDef.upperAngle = -(20 * DEGTORAD);
				revoluteJointDef.enableMotor = true;
				revoluteJointDef.maxMotorTorque = 12000.0f;
				m_mjoint2 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
				
				//m_mjoint2->SetMotorSpeed(-1.0f) ;//for test purpose
			}
			/** Back Side Pickup joined to backside arm 2.
			 *  many polygon shapes are added to the body fixture as we can't create 
			 * 	concave polygons using box2D.
			 */ 
			{	 
				//! Shape for the pickup.
				b2PolygonShape b1;
				b2Vec2 vertices[4];
				vertices[0].Set(0.0f, 3.0f);
				vertices[1].Set(-1.0f, 1.0f);
				vertices[2].Set(-1.5f, 2.0f);
				b1.Set(vertices, 3);
				
				//! Adds the shape to the fixture.
				b2FixtureDef fdb1;
				fdb1.shape = &b1;
				fdb1.density = 1.0f;
				fdb1.friction = 0;
				b2BodyDef bd1;
				bd1.type = b2_dynamicBody;
				bd1.position.Set(-25.0f, -17.0f);
				bd1.angle = 50* DEGTORAD;
				m_bpick = m_world->CreateBody(&bd1);
				m_bpick->CreateFixture(&fdb1);
				
				//! Another polygon shape is added to the fixture.
				vertices[0].Set(-1.5f, 0.2f);
				vertices[1].Set(-1.0f, 1.0f);
				vertices[2].Set(-1.5f, 2.0f);
				vertices[3].Set(-2.0f, 0.0f);
				b1.Set(vertices, 4);
				fdb1.shape = &b1;
				m_bpick->CreateFixture(&fdb1);
				
				//! Yet another polygon shape is added to the fixture.
				vertices[0].Set(-1.5f, 0.2f);
				vertices[1].Set(-1.0f, -2.0f);
				vertices[2].Set(-1.5f, -2.0f);
				vertices[3].Set(-2.0f, 0.0f);
				b1.Set(vertices, 4);
				fdb1.shape = &b1;
				m_bpick->CreateFixture(&fdb1);
				
				//! Yet another polygon shape is added to the fixture.
				vertices[0].Set(-1.0f, -2.0f);
				vertices[1].Set(-1.5f, -2.0f);
				vertices[2].Set(1.0f, -3.0f);
				b1.Set(vertices, 3);
				fdb1.shape = &b1;
				m_bpick->CreateFixture(&fdb1);
     		}
     		
     		/** The motorised joint which joins backside pickup with 2nd back arm.
     		 */ 
     		{
				//! joint using b2RevoluteJointDef
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_backarm2;
				revoluteJointDef.bodyB = m_bpick;
				revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(0,-13.5);
				revoluteJointDef.localAnchorB.Set(0,2.9);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = (30 * DEGTORAD);
				revoluteJointDef.upperAngle = (90 * DEGTORAD);
				revoluteJointDef.enableMotor = true;
				revoluteJointDef.maxMotorTorque = 7000.0f;
				m_mjoint4 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
				
				//m_mjoint4->SetMotorSpeed(-1.0f); //for test purpose
			}
			
			/** Frontarm1 arm attatched to chassis.
			 * 	The gravityScale for the body is set to 0.1f so that it does not automatically go down.
			 */ 
			{	
				//! front arm shape using b2PolygonShape
				b2PolygonShape frontarm1;
				frontarm1.SetAsBox(14,1);
				b2FixtureDef fdfrontarm1;
				fdfrontarm1.filter.groupIndex = -1;
				fdfrontarm1.shape = &frontarm1;
				fdfrontarm1.density = 1.0f;
				
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(14.0f, -5.0f);
				m_frontarm1 = m_world->CreateBody(&bd);
				m_frontarm1->CreateFixture(&fdfrontarm1);
				m_frontarm1->SetGravityScale(0.1f);
			}
			/** The motorised joint between frontarm1 and chassis.
			 */ 
			{	
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_car;
				revoluteJointDef.bodyB = m_frontarm1;
				revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(0,7);
				revoluteJointDef.localAnchorB.Set(-14,0);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = (-30* DEGTORAD);
				revoluteJointDef.upperAngle =  (0* DEGTORAD);
				revoluteJointDef.enableMotor = true;
				revoluteJointDef.maxMotorTorque = 7000.0f;
				m_mjoint5 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
			}
			
			/** Digger attached to the frontarm1.
			 */ 
			{
				//! digger shape using b2PolygonShape
				b2PolygonShape digger;
				b2Vec2 vertices[3];
				vertices[0].Set(0.0f,5.0f);
				vertices[1].Set(-2.0f,-6.0f);
				vertices[2].Set(2.0f,-4.0f);
				digger.Set(vertices,3);
				
				//! digger fixture using the above shape
				b2FixtureDef fddigger;
				fddigger.shape = &digger;
				fddigger.density = 1.0f;
				
				//! creates digger body using above fixture
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(28.0f, -5.0f);
				m_digger = m_world->CreateBody(&bd);
				m_digger->SetGravityScale(0.1f);
				m_digger->CreateFixture(&fddigger);
				
				//! another shape is added to the digger fixture.
				vertices[0].Set(-2.0f,-6.0f);
				vertices[1].Set(11.0f,-5.0f);
				vertices[2].Set(2.0f,-4.0f);
				digger.Set(vertices,3);
				m_digger->CreateFixture(&fddigger);
			
			}
			
			/**the revolute joint between frontarm1 and digger.
			 */ 
			{
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_frontarm1;
				revoluteJointDef.bodyB = m_digger;
				revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(14,0);
				revoluteJointDef.localAnchorB.Set(0,-1);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = (10* DEGTORAD);
				revoluteJointDef.upperAngle =  (50* DEGTORAD);
				revoluteJointDef.enableMotor = true;
				revoluteJointDef.maxMotorTorque = 7000.0f;
				m_mjoint6 = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
				
				//m_mjoint6->SetMotorSpeed(1.0f); //for test purpose
			}
			
			/** The piston for frontarm. 
			 * 	Piston is used to give a feel about how actual JCB diggers work.
			 *  A revolute joint is used so that the piston can rotate freely when lifting the digger.
			 */ 
			{
				//! a rectangular piston part.
				b2PolygonShape piston;
				piston.SetAsBox(3,1);
				b2FixtureDef fdpiston;
				fdpiston.filter.groupIndex = -1;
				fdpiston.shape = &piston;
				fdpiston.density = 2.0f;
				fdpiston.friction = 0;
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(3.0f, -10.0f);
				bd.angle = 20* DEGTORAD;
				m_piston1 = m_world->CreateBody(&bd);
				m_piston1->CreateFixture(&fdpiston);
				
				//! revolute joint between chassis and the piston part.
				b2RevoluteJointDef revoluteJointDef;
				revoluteJointDef.bodyA = m_car;
				revoluteJointDef.bodyB = m_piston1;
				revoluteJointDef.collideConnected = false;
				revoluteJointDef.localAnchorA.Set(0,0);
				revoluteJointDef.localAnchorB.Set(-3,0);
				revoluteJointDef.enableLimit = true;
				revoluteJointDef.lowerAngle = (10* DEGTORAD);
				revoluteJointDef.upperAngle =  (45* DEGTORAD);
				(b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
				
				//! Other part of the piston is created using longer length and small width.
				bd.position.Set(10.0f , -7.0f);
				piston.SetAsBox(5,0.5);
				m_piston2 = m_world->CreateBody(&bd);
				m_piston2->CreateFixture(&fdpiston);
				
				//! Distance joint between the piston and digger to lift it smoothly when piston is released. 
				b2DistanceJointDef jointDef;
				jointDef.bodyA = m_frontarm1;
				jointDef.bodyB = m_piston2;
				jointDef.localAnchorA.Set(-1,0);
				jointDef.localAnchorB.Set(4.5,0);
				jointDef.length = 0;
				jointDef.collideConnected = true;
				jointDef.dampingRatio = 0.5f;
				(b2DistanceJoint*)m_world->CreateJoint( &jointDef );
				
				//! Prismatic joint between the parts of the piston to lift the digger.
				b2PrismaticJointDef prismaticJointDef;
				prismaticJointDef.bodyA = m_piston1;
				prismaticJointDef.bodyB = m_piston2;
				prismaticJointDef.localAxisA.Set(1,0);
				prismaticJointDef.localAnchorA.Set(2.5,0);
				prismaticJointDef.localAnchorB.Set(-2.5,0);
				prismaticJointDef.enableMotor = true;
				prismaticJointDef.maxMotorForce = 7.0f;
				prismaticJointDef.enableLimit = true;
				prismaticJointDef.lowerTranslation = -3.5;
				prismaticJointDef.upperTranslation = 3;
				prismaticJointDef.collideConnected = false;
				m_pistonjoint = (b2PrismaticJoint *) m_world->CreateJoint( &prismaticJointDef);
				
				//m_pistonjoint->SetMaxMotorForce(5000.0f); //for test purpose
				//m_pistonjoint->SetMotorSpeed(10.0f);
				
			}	
		}
		//!testbox
		for(int i=0;i<10;i++)
		{
			b2PolygonShape testbox;
			testbox.SetAsBox(1.4,1.4);
			b2FixtureDef fdtestbox;
			fdtestbox.filter.groupIndex = 1;
			fdtestbox.shape = &testbox;
			fdtestbox.density = 0.4f;
			fdtestbox.friction=0.5;
			b2BodyDef bd;
			int testx = (rand() % 40) ;
			//cout << testx;
			bd.position.Set(20.0f * testx + 40.0f , 10.0f);
			bd.type = b2_dynamicBody;
			test_box1 = m_world->CreateBody(&bd);
			test_box1->CreateFixture(&fdtestbox);
		}
		//!Big bin to put the testboxes inside
		{
			b2PolygonShape bigbin1;
			b2PolygonShape bigbin2;
			b2PolygonShape bigbin3;
			
			b2Vec2 vertices1[4];
			b2Vec2 vertices2[4];
			b2Vec2 vertices3[4];
			vertices1[0].Set(0.0f, 0.0f);
			vertices1[3].Set(0.0f, -100.0f);
			vertices1[2].Set(10.0f, -100.0f);
			vertices1[1].Set(10.0f, 0.0f);
			bigbin1.Set(vertices1, 4);
			vertices2[0].Set(10.0f, -100.0f);
			vertices2[1].Set(10.0f, -90.0f);
			vertices2[2].Set(100.0f,-90.0f);
			vertices2[3].Set(100.0f, -100.0f);
			bigbin2.Set(vertices2, 4);
			vertices3[0].Set(100.0f, -100.0f);
			vertices3[1].Set(100.0f, 0.0f);
			vertices3[2].Set(110.0f, 0.0f);
			vertices3[3].Set(110.0f, -100.0f);
			bigbin3.Set(vertices3, 4);

			b2BodyDef bd2;
			bd2.type = b2_staticBody;
			bd2.position.Set(844.0f, y);
			b2Body* m_bigbin = m_world->CreateBody(&bd2);
			m_bigbin->CreateFixture(&bigbin1, 1.0f);
			m_bigbin->CreateFixture(&bigbin2, 1.0f);
			m_bigbin->CreateFixture(&bigbin3, 1.0f);
		}
		//!Small bin to collect the ball
		{
			b2PolygonShape bin1;
			b2PolygonShape bin2;
			b2PolygonShape bin3;
			
			b2Vec2 vertices1[4];
			b2Vec2 vertices2[4];
			b2Vec2 vertices3[4];
			vertices1[0].Set(0.0f, 0.0f);
			vertices1[1].Set(0.0f, 10.0f);
			vertices1[2].Set(2.0f, 10.0f);
			vertices1[3].Set(2.0f, 0.0f);
			bin1.Set(vertices1, 4);
			vertices2[0].Set(2.0f, 0.0f);
			vertices2[1].Set(2.0f, 2.0f);
			vertices2[2].Set(12.0f,2.0f);
			vertices2[3].Set(12.0f, 0.0f);
			bin2.Set(vertices2, 4);
			vertices3[0].Set(12.0f, 0.0f);
			vertices3[1].Set(12.0f, 10.0f);
			vertices3[2].Set(14.0f, 10.0f);
			vertices3[3].Set(14.0f, 0.0f);
			bin3.Set(vertices3, 4);
	
			b2BodyDef bd;
			bd.type = b2_staticBody;
			bd.position.Set(-100.0f, -20.0f);
			b2Body* m_bin = m_world->CreateBody(&bd);
			m_bin->CreateFixture(&bin1, 1.0f);
			m_bin->CreateFixture(&bin2, 1.0f);
			m_bin->CreateFixture(&bin3, 1.0f);
		}
		
		//!testball
		{
			b2CircleShape circle;
			circle.m_radius = 1.1f;
			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 1.5f;
			fd.friction = 0.9f;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-35.0f, -16.5f);
			test_ball = m_world->CreateBody(&bd);
			test_ball->CreateFixture(&fd);
		}
		
	}
	/** mouse press event callback function.
	 * 	generates obstacles when mouse is pressed somewhere on the screen.
	 * 	\sa callbacks_t::mousedown_cb()
	 * 	\param p the point at which mouse is pressed
	 */ 
    void dominos_t::mouse_down(const b2Vec2& p) {
				b2PolygonShape testbox;
				testbox.SetAsBox(1.4,1.4);
				b2FixtureDef fdtestbox;
				fdtestbox.filter.groupIndex = 1;
				fdtestbox.shape = &testbox;
				fdtestbox.density = 0.3f;
				fdtestbox.friction=0.5;
				b2Body* box=NULL;
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(p.x, p.y);
				box = m_world->CreateBody(&bd);
				box->CreateFixture(&fdtestbox);
	}
	/** Keyboard press event callback function.
	 * 	Handles keyboard press event callbacks other than default in callbacks_t::keyboard_cb()
	 * 	\sa keyboard_up(), callbacks_t::keyboard_cb()
	 * 	\param key the key pressed
	 */ 
    void dominos_t::keyboard(unsigned char key)
    {
		float x_car,y_car;
        switch (key)
		{
		case 'a':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(m_speed);
			callbacks_t::keyboard_dominos_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;

		case 'd':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(-m_speed);
			callbacks_t::keyboard_dominos_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;
		
		case '1':
			m_mjoint1->SetMotorSpeed(-1.0f);
			break;	
			
		case '2':
			m_mjoint1->SetMotorSpeed(0.2f);
			break;
			
		case '3':
			m_mjoint2->SetMotorSpeed(-1.0f);
			break;	
		
		case '4':
			m_mjoint2->SetMotorSpeed(0.3f);
			break;
			
		case '5':
			m_mjoint4->SetMotorSpeed(-1.0f);
			break;
			
		case '6':	
			m_mjoint4->SetMotorSpeed(1.0f);
			break;
			
		case '7':
			m_pistonjoint->SetMaxMotorForce(5000.0f);
			m_pistonjoint->SetMotorSpeed(10.0f);
			break;
			
		case '8':	
			m_pistonjoint->SetMaxMotorForce(5000.0f);
			m_pistonjoint->SetMotorSpeed(-10.0f);
			break;
		
		case '9':	
			m_mjoint6->SetMotorSpeed(-1.0f);
			break;
		
		case '0':	
			m_mjoint6->SetMotorSpeed(1.0f);
			break;	
		}
    }
    /** Keyboard release event callback function.
	 * 	Handles keyboard release event callbacks other than default in callbacks_t::keyboard_up_cb()
	 * 	\sa keyboard(), callbacks_t::keyboard_up_cb()
	 * 	\param key the key pressed
	 */ 
	void dominos_t::keyboard_up(unsigned char key)
    {
		float x_car,y_car;
        switch (key)
		{
		case 'a':
		case 'd':
			x_car=m_car->GetPosition().x;
			y_car=m_car->GetPosition().y;
			m_spring1->SetMotorSpeed(0.0f);
			callbacks_t::keyboard_dominos_cb(GLUT_KEY_LEFT,x_car,y_car);
			break;
		
		case '1':			
		case '2':
			m_mjoint1->SetMotorSpeed(0.0f);
			break;
			
		case '3':
		case '4':
			m_mjoint2->SetMotorSpeed(0.0f);
			break;
			
		case '5':
		case '6':	
			m_mjoint4->SetMotorSpeed(0.0f);
			break;
			
		case '7':
		case '8':
			m_pistonjoint->SetMotorSpeed(0.0f);
			break;
		
		case '9':
		case '0':	
			m_mjoint6->SetMotorSpeed(0.0f);
			break;	
		}
	}
  //! creates new sim_t
  sim_t *sim = new sim_t("Dominos", dominos_t::create);
}
