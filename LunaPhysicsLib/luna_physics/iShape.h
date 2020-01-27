#pragma once

#include "eShapeType.h"

namespace phys
{
	class iShape
	{
	public:
		virtual ~iShape() {}
		inline const eShapeType& GetShapeType() { return mShapeType; }

	protected:
		iShape(eShapeType shapeType)
			: mShapeType(shapeType)
		{

		}

	private:
		eShapeType mShapeType;

		// private so as to not be used. like, ever.
		iShape() = delete;
		iShape(const iShape& other) = delete;
		iShape& operator=(const iShape& other) = delete;
	};
}