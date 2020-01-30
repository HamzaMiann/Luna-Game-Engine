
#include <Behaviour/iBehaviour.h>
#include <interfaces/physics/iPhysicsComponent.h>

namespace AI {

  class iAIBehaviour : public iBehaviour
  {
  public:
    virtual ~iAIBehaviour() {}
  protected:
    iAIBehaviour(iObject* parent)
      : iBehaviour(parent)
    {}

  };

}
