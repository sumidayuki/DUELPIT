#include "Keyboard.h"


void Keyboard::StaticConstructor()
{
    Update();
}


void Keyboard::StaticDestructor()
{

}


void Keyboard::Update()
{
	m_isPressedAnyKey = false;

	unsigned char states[256];

	if (GetKeyboardState(states))
	{
		m_isConnected = true;

        for (int i = 0; i < 256; i++)
        {
            const bool current = (states[i] & 0x80) != 0;

            m_keys[i].Update(current);

            if (!IsPressAnyAllowedKey(i))
            {
                continue;
            }

            if (m_keys[i].WasPressedThisFrame())
            {
                m_isPressedAnyKey = true;
            }
        }
	}
	else
	{
		m_isConnected = false;
	}
}

bool Keyboard::IsPressAnyAllowedKey(int key)
{
    // PC‘€ì—pƒL[
    if (key >= VK_F1 && key <= VK_F12)
    {
        return false;
    }

    switch (key)
    {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:

        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_CONTROL:

        case VK_LMENU:
        case VK_RMENU:
        case VK_MENU:

        case VK_LWIN:
        case VK_RWIN:

        case VK_CAPITAL:
        case VK_NUMLOCK:
        case VK_SCROLL:

        case VK_SNAPSHOT:   // PrintScreen
        case VK_PAUSE:
        case VK_APPS:       // Menu
            return false;
    }

    return true;
}


const ButtonControl& Keyboard::GetKeyState(KeyCode key)
{
    return m_keys[(int)key];
}
