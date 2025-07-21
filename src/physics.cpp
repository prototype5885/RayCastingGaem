#include "physics.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "utils.h"

#include <cmath>

// ai written
bool physics::PlayerCollisionCheck(const geometry::Vector2 desiredPlayerPos) {
  using namespace geometry;
  using namespace level;
  Vector2 newPlayerPos = desiredPlayerPos;
  bool collisionOccurred = false;

  for (size_t s = 0; s < currentLevel.sectors.size(); s++) {
    const vector<Wall> &walls = currentLevel.sectors[s].walls;
    for (size_t i = 0; i < walls.size(); i++) {
      const Wall &wall = walls[i];

      if (!wall.collision)
        continue;

      // 1. Calculate the closest point on the wall segment to the player's *desired* position
      float closestX = wall.a;
      float closestY = wall.b;

      const float dx = wall.c - wall.a;
      const float dy = wall.d - wall.b;
      const float lengthSquared = dx * dx + dy * dy;

      if (lengthSquared != 0) { // Avoid division by zero for point-like walls
        float t = ((newPlayerPos.x - wall.a) * dx + (newPlayerPos.y - wall.b) * dy) / lengthSquared;
        t = utils::clamp(t, 0.0f, 1.0f); // Clamp t to [0, 1] for segment
        closestX = wall.a + t * dx;
        closestY = wall.b + t * dy;
      }

      // 2. Calculate the distance from the desired player position to this closest point on the wall
      const Vector2 closestPointOnWall = {closestX, closestY};
      const float distToWall = EuclideanDistance(newPlayerPos, closestPointOnWall);

      // 3. Check for collision: Is the distance less than the player's radius?
      if (distToWall < player::radius) {
        collisionOccurred = true;

        // 4. Resolve the collision: Push the player out of the wall
        // Calculate the penetration depth
        const float penetrationDepth = player::radius - distToWall;

        // Calculate the normal vector from the wall pointing towards the player
        // This is the direction to push the player
        Vector2 normal = {0, 0};
        if (distToWall > 0) { // Avoid division by zero if player is exactly on the point
          normal.x = (newPlayerPos.x - closestPointOnWall.x) / distToWall;
          normal.y = (newPlayerPos.y - closestPointOnWall.y) / distToWall;
        } else {
          // If distToWall is 0, the player is exactly on the wall.
          // This case needs careful handling. For simplicity, we can
          // push along an arbitrary direction or just prevent movement.
          // A more robust solution for edge cases here involves looking at
          // the wall's actual normal, but for a simple push-out,
          // any non-zero direction works to get it moving.
          // For simplicity, let's just make sure it's not a zero vector.
          // For example, if playerPos is exactly wall.p1:
          if (wall.a == newPlayerPos.x && wall.b == newPlayerPos.y) {
            // Push along an arbitrary direction or based on movement intent
            normal = {1.0f, 0.0f}; // Or some other default
          } else {
            // This case implies playerPos == closestPointOnWall but distToWall is 0.
            // This is a rare floating point edge case, but we need a valid normal.
            // A better solution would rely on the wall's true normal.
            // For a line segment, the normal can be derived from (dy, -dx) or (-dy, dx)
            // Let's assume (dy, -dx) is one normal, and normalize it.
            const float normal_dx = wall.d - wall.b;
            const float normal_dy = -(wall.c - wall.a);
            const float normal_len = std::sqrt(normal_dx * normal_dx + normal_dy * normal_dy);
            if (normal_len > 0) {
              normal.x = normal_dx / normal_len;
              normal.y = normal_dy / normal_len;
            } else {
              normal = {1.0f, 0.0f}; // Fallback for zero-length walls
            }
            // You might need to flip the normal direction based on which side the player is on.
            // A dot product with player's desired movement vector can help here.
          }
        }

        // Move the player back by the penetration depth along the normal
        newPlayerPos.x += normal.x * penetrationDepth;
        newPlayerPos.y += normal.y * penetrationDepth;

        // You can also consider "sliding" along the wall if there's velocity
        // This is more advanced: calculate the remaining velocity component
        // perpendicular to the normal and apply it.
        // E.g., Vector2 remainingVelocity = desiredVelocity - dot(desiredVelocity, normal) * normal;
        // Then apply remainingVelocity.
      }
    }
  }

  // Update the player's actual position if there were no collisions or after resolution
  // If you are doing sliding, you might apply partial movements.
  // For basic push-out, just set the position.
  player::pos = newPlayerPos;

  return collisionOccurred;
}
