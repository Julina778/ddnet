#ifndef GAME_CLIENT_COMPONENTS_ALESSTYA_H
#define GAME_CLIENT_COMPONENTS_ALESSTYA_H

#include <game/client/component.h>
#include <engine/console.h>

class CAlesstya : public CComponent
{
public:
	CAlesstya();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
};

#endif
