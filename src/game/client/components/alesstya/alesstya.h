#ifndef GAME_CLIENT_COMPONENTS_ALESSTYA_H
#define GAME_CLIENT_COMPONENTS_ALESSTYA_H

#include <engine/console.h>
#include <game/client/component.h>

class CAlesstya : public CComponent
{
public:
	CAlesstya();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
};

#endif
