#pragma once

struct UINavigation : IComponentData
{
	uint64_t up = -1;
	uint64_t down = -1;
	uint64_t left = -1;
	uint64_t right = -1;
};