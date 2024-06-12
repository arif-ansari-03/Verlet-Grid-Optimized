#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>

using namespace std;

float length(sf::Vector2f a)
{
    return sqrt(a.x*a.x + a.y*a.y);
}

struct particle
{
    sf::Vector2f position;
    sf::Vector2f old_position;
    sf::Vector2f acceleration;
    float radius;
    sf::CircleShape shape;
    sf::Vector2f rr;

    particle()
    {
        position = {400.f, 400.f};
        old_position = position;
        radius = 10.f;
        rr = {radius, radius};
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(position-rr);
    }

    particle(sf::Vector2f pos, float rad)
    {
        position = pos;
        old_position = position;
        radius = rad;
        rr = {radius, radius};
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(position-rr);
    }

    void update_velocity(sf::Vector2f v, float dt)
    {
        old_position = position - v * dt;
    }

    void update(float dt)
    {
        const sf::Vector2f velocity = position - old_position;

        old_position = position;

        position = position + velocity + acceleration * dt * dt;

        acceleration = {};

        shape.setPosition(position - rr);
    }

    void accelerate(sf::Vector2f acc)
    {
        acceleration += acc;
    }
};

vector<int> grid[300][300];


struct Solver
{
    sf::Vector2f gravity = {0.f, 5.f};
    vector<particle> P;
    float radius = 10.f;
    int n;
    float border_x = 900.f, border_y = 900.f;
    
    Solver(int a)
    {
        P = vector<particle>(a);
        n = a;
    }

    void add_particle(sf::Vector2f pos)
    {
        P.emplace_back(particle(pos, radius));
        n++;
    }

    void apply_gravity()
    {
        for (auto &particle : P)
        particle.accelerate(gravity);
    }

    void update_position(float dt)
    {
        for (auto &particle: P)
        particle.update(dt);
    }

    void apply_constraint()
    {
        // sf::Vector2f center = {450.f, 450.f};
        // float radius = 200.f;

        // for (auto &particle: P)
        // {
        //     sf::Vector2f to_obj = particle.position - center;
        //     float dist = length(to_obj);

        //     if (dist > radius - particle.radius)
        //     {
        //         sf::Vector2f n = to_obj / length(to_obj) * (radius - particle.radius);
        //         particle.position = center + n;
        //     }
        // }

        for (auto &particle: P)
        {
            particle.position.x = min(particle.position.x, border_x - particle.radius);
            particle.position.x = max(particle.position.x, particle.radius);
            particle.position.y = min(particle.position.y, border_y - particle.radius);
            particle.position.y = max(particle.position.y, particle.radius);
        }
    }

    void solve_cell_and_particle(vector<int> &_grid, particle &particle)
    {
        for (int &i : _grid)
        {
            sf::Vector2f col_axis = P[i].position - particle.position;
            float dist = length(col_axis);
            if (dist < P[i].radius + particle.radius)
            {
                col_axis = col_axis / dist;
                dist = P[i].radius + particle.radius - dist;
                P[i].position = P[i].position + 0.5f * dist * col_axis;
                particle.position = particle.position - 0.5f * dist * col_axis;
            }
        }
    }

    void solve_collisions()
    {
        int grid_size = 2 * radius;

        vector<pair<int, int>> temp;

        for (int m = 0; m < n; m++)
        {
            particle &particle = P[m];
            int x = 1+particle.position.x, y = 1+particle.position.y;
            int i = x / grid_size, j = y / grid_size;
            ++i, ++j;
            ++i, ++j;


            solve_cell_and_particle(grid[i-1][j-1], particle);
            solve_cell_and_particle(grid[i-1][j], particle);
            solve_cell_and_particle(grid[i-1][j+1], particle);
            solve_cell_and_particle(grid[i][j-1], particle);
            solve_cell_and_particle(grid[i][j], particle);
            solve_cell_and_particle(grid[i][j+1], particle);
            solve_cell_and_particle(grid[i+1][j-1], particle);
            solve_cell_and_particle(grid[i+1][j], particle);
            solve_cell_and_particle(grid[i+1][j+1], particle);
            grid[i][j].emplace_back(m);
            temp.emplace_back(make_pair(i, j));
        }

        for (auto &[i, j] : temp) if (!grid[i][j].empty()) grid[i][j].clear();
    }

    void update(float dt)
    {
        int num_sub_steps = 1;
        float sub_step = dt / num_sub_steps;
        for (int i = 0; i < num_sub_steps; i++)
        {
            apply_gravity();
            apply_constraint();
            solve_collisions();
            update_position(sub_step);
        }
    }
};