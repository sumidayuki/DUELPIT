#include "PlayerCameraSystem.h"
#include "PlayerCamera.h"
#include "DebugManager.h"

void PlayerCameraSystem::Start(World& world)
{
    m_yaw = 0.0f;
    m_pitch = 15.0f;
}

void PlayerCameraSystem::Draw(World& world)
{
    View<Transform, PlayerCamera> view(world);

    for (auto [entity, transform, playerCamera] : view)
    {
        if (!m_playerTransform)
        {
            m_playerTransform = world.GetComponent<Transform>(playerCamera.player);
        }

        const InputDeviceType deviceType = InputManager::GetCurrentInputDeviceType();

        playerCamera.currentScale = Mathf::Lerp(
            playerCamera.currentScale,
            playerCamera.targetScale,
            Time::GetDeltaTime() * playerCamera.zoomLerpFactor
        );

        Vector2 cameraInput = Vector2::zero;

        switch (deviceType)
        {
            // キーボードマウス
            case InputDeviceType::Keyboard_Mouse:
            {
                cameraInput = Mouse::GetVelocity();

                playerCamera.yaw -= cameraInput.x * playerCamera.sensitivity;

                playerCamera.pitch = Mathf::Clamp(
                    playerCamera.pitch +
                    cameraInput.y * playerCamera.sensitivity,
                    -40.0f,
                    80.0f
                );

                break;
            }
            // ゲームパッド
            case InputDeviceType::Gamepad:
            {
                const Gamepad* gamepad = InputManager::GetGamepad(0);

                if (gamepad && gamepad->IsConnected())
                {
                    const StickControl& rightStick = gamepad->RightStick();
                    const float x = rightStick.X().GetValue();
                    const float y = -rightStick.Y().GetValue();

                    constexpr float gamepadCameraSpeed = 120.0f;

                    const float deltaTime = Time::GetDeltaTime();

                    playerCamera.yaw -= x * gamepadCameraSpeed * deltaTime;
                    playerCamera.pitch = Mathf::Clamp(
                        playerCamera.pitch +
                        y *
                        gamepadCameraSpeed *
                        deltaTime,
                        -40.0f,
                        80.0f
                    );
                }

                break;
            }
        }

        const float yawRad = -playerCamera.yaw * Mathf::Deg2Rad;
        const float pitchRad = playerCamera.pitch * Mathf::Deg2Rad;
        const float scale = playerCamera.currentScale;
        const float side = playerCamera.offset.x * scale;
        const float height = playerCamera.offset.y * scale;
        const float distance = -playerCamera.offset.z * scale;

        const Vector3 cameraRight(Mathf::Cos(yawRad), 0.0f, -Mathf::Sin(yawRad));
        const Vector3 cameraForward(Mathf::Sin(yawRad), 0.0f, Mathf::Cos(yawRad));

        const Vector3 lookAtPoint = m_playerTransform->position + Vector3(0.0f, height, 0.0f) + cameraRight * side;
        const Vector3 horizontalOffset = -cameraForward * (distance * Mathf::Cos(pitchRad));
        const Vector3 verticalOffset(0.0f, distance * Mathf::Sin(pitchRad), 0.0f);

        Vector3 relativePos = horizontalOffset + verticalOffset;
        Vector3 targetPosition = lookAtPoint + relativePos;

        const Vector3 cameraDirection = relativePos.Normalized();

        Ray ray(lookAtPoint, cameraDirection);
        RaycastHit hit;

        if (Physics::Raycast(world, ray, hit, distance, Layers::Environment))
        {
            constexpr float cameraCollisionOffset = 0.1f;
            targetPosition = hit.point - cameraDirection * cameraCollisionOffset;
        }
        TransformAPI::SetLocalPosition(transform, targetPosition);

        const Vector3 lookDirection = (lookAtPoint - targetPosition).Normalized();
        TransformAPI::SetLocalRotation(transform, Quaternion::LookRotation(lookDirection, Vector3::up));
    }
}