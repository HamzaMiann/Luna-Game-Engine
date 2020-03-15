#pragma once


enum class eBodyType
{
	rigid,
	soft
};

class iBody
{
public:
	iBody() = delete;
	virtual ~iBody() {}

	inline eBodyType GetBodyType() { return _bodyType; }

private:
	eBodyType _bodyType;

protected:

	iBody(eBodyType type):
		_bodyType(type)
	{ }
};