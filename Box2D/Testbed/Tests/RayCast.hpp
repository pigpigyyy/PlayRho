/*
* Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
* Modified work Copyright (c) 2016 Louis Langholtz https://github.com/louis-langholtz/Box2D
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

#ifndef RAY_CAST_H
#define RAY_CAST_H

// This test demonstrates how to use the world ray-cast feature.
// NOTE: we are intentionally filtering one of the polygons, therefore
// the ray will always miss one type of polygon.

namespace box2d {

// This callback finds the closest hit. Polygon 0 is filtered.
class RayCastClosestCallback : public RayCastFixtureReporter
{
public:
	RayCastClosestCallback()
	{
		m_hit = false;
	}

	float_t ReportFixture(Fixture* fixture, const Vec2& point, const UnitVec2& normal, float_t fraction) override
	{
		Body* body = fixture->GetBody();
		void* userData = body->GetUserData();
		if (userData)
		{
			int32 index = *(int32*)userData;
			if (index == 0)
			{
				// By returning -1, we instruct the calling code to ignore this fixture and
				// continue the ray-cast to the next fixture.
				return -1.0f;
			}
		}

		m_hit = true;
		m_point = point;
		m_normal = normal;

		// By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}
	
	bool m_hit;
	Vec2 m_point;
	UnitVec2 m_normal;
};

// This callback finds any hit. Polygon 0 is filtered. For this type of query we are usually
// just checking for obstruction, so the actual fixture and hit point are irrelevant. 
class RayCastAnyCallback : public RayCastFixtureReporter
{
public:
	RayCastAnyCallback()
	{
		m_hit = false;
	}

	float_t ReportFixture(Fixture* fixture, const Vec2& point, const UnitVec2& normal, float_t fraction) override
	{
		Body* body = fixture->GetBody();
		void* userData = body->GetUserData();
		if (userData)
		{
			int32 index = *(int32*)userData;
			if (index == 0)
			{
				// By returning -1, we instruct the calling code to ignore this fixture
				// and continue the ray-cast to the next fixture.
				return -1.0f;
			}
		}

		m_hit = true;
		m_point = point;
		m_normal = normal;

		// At this point we have a hit, so we know the ray is obstructed.
		// By returning 0, we instruct the calling code to terminate the ray-cast.
		return 0.0f;
	}

	bool m_hit;
	Vec2 m_point;
	UnitVec2 m_normal;
};

// This ray cast collects multiple hits along the ray. Polygon 0 is filtered.
// The fixtures are not necessary reported in order, so we might not capture
// the closest fixture.
class RayCastMultipleCallback : public RayCastFixtureReporter
{
public:
	enum
	{
		e_maxCount = 3
	};

	RayCastMultipleCallback()
	{
		m_count = 0;
	}

	float_t ReportFixture(Fixture* fixture, const Vec2& point, const UnitVec2& normal, float_t fraction) override
	{
		Body* body = fixture->GetBody();
		void* userData = body->GetUserData();
		if (userData)
		{
			int32 index = *(int32*)userData;
			if (index == 0)
			{
				// By returning -1, we instruct the calling code to ignore this fixture
				// and continue the ray-cast to the next fixture.
				return -1.0f;
			}
		}

		assert(m_count < e_maxCount);

		m_points[m_count] = point;
		m_normals[m_count] = normal;
		++m_count;

		if (m_count == e_maxCount)
		{
			// At this point the buffer is full.
			// By returning 0, we instruct the calling code to terminate the ray-cast.
			return 0.0f;
		}

		// By returning 1, we instruct the caller to continue without clipping the ray.
		return 1.0f;
	}

	Vec2 m_points[e_maxCount];
	UnitVec2 m_normals[e_maxCount];
	int32 m_count;
};


class RayCast : public Test
{
public:

	enum
	{
		e_maxBodies = 256
	};

	enum Mode
	{
		e_closest,
		e_any,
		e_multiple
	};

	RayCast()
	{
		// Ground body
		{
			BodyDef bd;
			Body* ground = m_world->CreateBody(bd);

			EdgeShape shape;
			shape.Set(Vec2(-40.0f, 0.0f), Vec2(40.0f, 0.0f));
			ground->CreateFixture(FixtureDef{&shape, 0.0f});
		}

		{
			m_polygons[0].Set({Vec2(-0.5f, 0.0f), Vec2(0.5f, 0.0f), Vec2(0.0f, 1.5f)});
		}

		{
			m_polygons[1].Set({Vec2(-0.1f, 0.0f), Vec2(0.1f, 0.0f), Vec2(0.0f, 1.5f)});
		}

		{
			float_t w = 1.0f;
			float_t b = w / (2.0f + Sqrt(2.0f));
			float_t s = Sqrt(2.0f) * b;

			m_polygons[2].Set({
				Vec2(0.5f * s, 0.0f),
				Vec2(0.5f * w, b),
				Vec2(0.5f * w, b + s),
				Vec2(0.5f * s, w),
				Vec2(-0.5f * s, w),
				Vec2(-0.5f * w, b + s),
				Vec2(-0.5f * w, b),
				Vec2(-0.5f * s, 0.0f)
			});
		}

		{
			m_polygons[3].SetAsBox(0.5f, 0.5f);
		}

		{
			m_circle.SetRadius(0.5);
		}

		{
			m_edge.Set(Vec2(-1.0f, 0.0f), Vec2(1.0f, 0.0f));
		}

		m_bodyIndex = 0;
		memset(m_bodies, 0, sizeof(m_bodies));

		m_angle = 0.0f;

		m_mode = e_closest;
	}

	void Create(int32 index)
	{
		if (m_bodies[m_bodyIndex] != nullptr)
		{
			m_world->Destroy(m_bodies[m_bodyIndex]);
			m_bodies[m_bodyIndex] = nullptr;
		}

		BodyDef bd;

		float_t x = RandomFloat(-10.0f, 10.0f);
		float_t y = RandomFloat(0.0f, 20.0f);
		bd.position = Vec2(x, y);
		bd.angle = 1_rad * RandomFloat(-Pi, Pi);

		m_userData[m_bodyIndex] = index;
		bd.userData = m_userData + m_bodyIndex;

		if (index == 4)
		{
			bd.angularDamping = 0.02f;
		}

		m_bodies[m_bodyIndex] = m_world->CreateBody(bd);

		if (index < 4)
		{
			FixtureDef fd;
			fd.shape = m_polygons + index;
			fd.friction = 0.3f;
			m_bodies[m_bodyIndex]->CreateFixture(fd);
		}
		else if (index < 5)
		{
			FixtureDef fd;
			fd.shape = &m_circle;
			fd.friction = 0.3f;

			m_bodies[m_bodyIndex]->CreateFixture(fd);
		}
		else
		{
			FixtureDef fd;
			fd.shape = &m_edge;
			fd.friction = 0.3f;

			m_bodies[m_bodyIndex]->CreateFixture(fd);
		}

		m_bodyIndex = (m_bodyIndex + 1) % e_maxBodies;
	}

	void Destroy()
	{
		for (int32 i = 0; i < e_maxBodies; ++i)
		{
			if (m_bodies[i] != nullptr)
			{
				m_world->Destroy(m_bodies[i]);
				m_bodies[i] = nullptr;
				return;
			}
		}
	}

	void Keyboard(Key key) override
	{
		switch (key)
		{
		case Key_1:
		case Key_2:
		case Key_3:
		case Key_4:
		case Key_5:
		case Key_6:
			Create(key - Key_1);
			break;

		case Key_D:
			Destroy();
			break;

		case Key_M:
			if (m_mode == e_closest)
			{
				m_mode = e_any;
			}
			else if (m_mode == e_any)
			{
				m_mode = e_multiple;
			}
			else if (m_mode == e_multiple)
			{
				m_mode = e_closest;
			}
			break;

		default:
			break;
		}
	}

	void PostStep(const Settings& settings, Drawer& drawer) override
	{
		drawer.DrawString(5, m_textLine, "Press 1-6 to drop stuff, m to change the mode");
		m_textLine += DRAW_STRING_NEW_LINE;
		switch (m_mode)
		{
		case e_closest:
			drawer.DrawString(5, m_textLine, "Ray-cast mode: closest - find closest fixture along the ray");
			break;
		
		case e_any:
			drawer.DrawString(5, m_textLine, "Ray-cast mode: any - check for obstruction");
			break;

		case e_multiple:
			drawer.DrawString(5, m_textLine, "Ray-cast mode: multiple - gather multiple fixtures");
			break;
		}

		m_textLine += DRAW_STRING_NEW_LINE;

		const auto L = 11.0f;
		const auto point1 = Vec2(0.0f, 10.0f);
		const auto d = Vec2(L * cosf(m_angle), L * sinf(m_angle));
		const auto point2 = point1 + d;

		if (m_mode == e_closest)
		{
			RayCastClosestCallback callback;
			m_world->RayCast(&callback, point1, point2);

			if (callback.m_hit)
			{
				drawer.DrawPoint(callback.m_point, 5.0f, Color(0.4f, 0.9f, 0.4f));
				drawer.DrawSegment(point1, callback.m_point, Color(0.8f, 0.8f, 0.8f));
				Vec2 head = callback.m_point + 0.5f * callback.m_normal;
				drawer.DrawSegment(callback.m_point, head, Color(0.9f, 0.9f, 0.4f));
			}
			else
			{
				drawer.DrawSegment(point1, point2, Color(0.8f, 0.8f, 0.8f));
			}
		}
		else if (m_mode == e_any)
		{
			RayCastAnyCallback callback;
			m_world->RayCast(&callback, point1, point2);

			if (callback.m_hit)
			{
				drawer.DrawPoint(callback.m_point, 5.0f, Color(0.4f, 0.9f, 0.4f));
				drawer.DrawSegment(point1, callback.m_point, Color(0.8f, 0.8f, 0.8f));
				Vec2 head = callback.m_point + 0.5f * callback.m_normal;
				drawer.DrawSegment(callback.m_point, head, Color(0.9f, 0.9f, 0.4f));
			}
			else
			{
				drawer.DrawSegment(point1, point2, Color(0.8f, 0.8f, 0.8f));
			}
		}
		else if (m_mode == e_multiple)
		{
			RayCastMultipleCallback callback;
			m_world->RayCast(&callback, point1, point2);
			drawer.DrawSegment(point1, point2, Color(0.8f, 0.8f, 0.8f));

			for (int32 i = 0; i < callback.m_count; ++i)
			{
				const auto p = callback.m_points[i];
				const auto n = callback.m_normals[i];
				drawer.DrawPoint(p, 5.0f, Color(0.4f, 0.9f, 0.4f));
				drawer.DrawSegment(point1, p, Color(0.8f, 0.8f, 0.8f));
				Vec2 head = p + 0.5f * n;
				drawer.DrawSegment(p, head, Color(0.9f, 0.9f, 0.4f));
			}
		}

		const auto advanceRay = !settings.pause || settings.singleStep;
		if (advanceRay)
		{
			m_angle += 0.25f * Pi / 180.0f;
		}

#if 0
		// This case was failing.
		{
			Vec2 vertices[4];
			//vertices[0] = Vec2(-22.875f, -3.0f);
			//vertices[1] = Vec2(22.875f, -3.0f);
			//vertices[2] = Vec2(22.875f, 3.0f);
			//vertices[3] = Vec2(-22.875f, 3.0f);

			PolygonShape shape;
			//shape.Set(vertices, 4);
			shape.SetAsBox(22.875f, 3.0f);

			RayCastInput input;
			input.p1 = Vec2(10.2725f,1.71372f);
			input.p2 = Vec2(10.2353f,2.21807f);
			//input.maxFraction = 0.567623f;
			input.maxFraction = 0.56762173f;

			Transformation xf;
			xf.SetIdentity();
			xf.position = Vec2(23.0f, 5.0f);

			RayCastOutput output;
			bool hit;
			hit = shape.RayCast(&output, input, xf);
			hit = false;

			Color color(1.0f, 1.0f, 1.0f);
			Vec2 vs[4];
			for (int32 i = 0; i < 4; ++i)
			{
				vs[i] = Transform(shape.m_vertices[i], xf);
			}

			drawer.DrawPolygon(vs, 4, color);
			drawer.DrawSegment(input.p1, input.p2, color);
		}
#endif
	}

	static Test* Create()
	{
		return new RayCast;
	}

	int32 m_bodyIndex;
	Body* m_bodies[e_maxBodies];
	int32 m_userData[e_maxBodies];
	PolygonShape m_polygons[4];
	CircleShape m_circle;
	EdgeShape m_edge;

	float_t m_angle;

	Mode m_mode;
};

} // namespace box2d

#endif