#include "cIntegrator.h"

namespace Integration
{
	struct State
	{
		State() {}
		State(glm::vec3& pos, glm::vec3& vel) : x(pos), v(vel) {}
		glm::vec3 x = glm::vec3(0.f);
		glm::vec3 v = glm::vec3(0.f);
	};

	struct Derivative
	{
		Derivative() {}
		Derivative(glm::vec3& pos, glm::vec3& vel) : dx(pos), dv(vel) {}
		glm::vec3 dx = glm::vec3(0.f);
		glm::vec3 dv = glm::vec3(0.f);
	};

	glm::vec3 acceleration(const State& state, double t)
	{
		const float k = 1.f;//15.0f;
		const float b = 1.f;// 0.1f;
		return -k * state.x - b * state.v;
	}

	Derivative evaluate(const State& initial,
		double t,
		float dt,
		const Derivative& d)
	{
		State state;
		state.x = initial.x + d.dx * dt;
		state.v = initial.v + d.dv * dt;

		Derivative output;
		output.dx = state.v;
		output.dv = acceleration(state, t + dt);
		return output;
	}

	void integrate(State& state,
		double t,
		float dt)
	{
		Derivative a, b, c, d;

		a = evaluate(state, t, 0.0f, Derivative());
		b = evaluate(state, t, dt * 0.5f, a);
		c = evaluate(state, t, dt * 0.5f, b);
		d = evaluate(state, t, dt, c);

		glm::vec3 dxdt = 1.0f / 6.0f *
			(a.dx + 2.0f * (b.dx + c.dx) + d.dx);

		glm::vec3 dvdt = 1.0f / 6.0f *
			(a.dv + 2.0f * (b.dv + c.dv) + d.dv);

		//state.x = state.x * dt;
		//state.v = state.v * dt;
		state.x = state.x + dxdt * dt;
		state.v = state.v + dvdt * dt;
	}
}

void cIntegrator::RK4(glm::vec3& x, glm::vec3& v, glm::vec3& a, glm::vec3& g, float gf, float dt, float t)
{
	v += a * dt;
	v += g * dt * gf;

	Integration::State xstate(x, v);
	Integration::integrate(xstate, t, dt);

	x = xstate.x;
}

void cIntegrator::Euler(glm::vec3& x, glm::vec3& v, glm::vec3& a, const glm::vec3& g, float gf, float dt)
{
	v += a * dt;
	v += g * dt * gf;
	x += v * dt;
}