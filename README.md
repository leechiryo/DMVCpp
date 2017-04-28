# DMVCpp

A C++ MVC framework for windows application.

It used some presentation technichs in the game so that d2d animations and special effects
can be added easily.

Also, the MVC framework make the application can be maintainted in an easy way and the C++ 
ensure the application's high performance.

## How to implement the animation.

An animation can be considered as a function which has an int parameter as frame index,
and return a pointer to point out which resource should be used to draw on the screen
at the specific frame.

A resource can be, for example, a pair of float values to indicate the element's position,
or an int value to indicate the element's color, or a new bitmap image of the element, and
anything else which can change the image on the screen.

```C++
template<typename T>
class Animation{
private:
  int m_frameIdx;
  PlayStatus m_status;
  PlayMode m_mode;
  T* m_resource;  // m_resource will point to the changing resource.

public:
  // should implement it in the inherited class to update the m_resource for every frame.
  virtual bool Update(int frame) = 0;

  void ConnectTo(T* changingResource){
    m_resource = changingResource;
  }
}

// This is a method of Controls.
template<typename T>
void SetAnimation(T* changingResource, Animation<T> *ani){
  // 1. Connect the ani to the changing resource.
  ani->ConnectTo(changingResource);

  // 2. Add the ani obj to global vector.
  App::Animations.add(ani);
}
```

Every time the frame changed, the engine will call all update method of the Animation objects,
which are stored in the global vector. Thus the resource will be updated, and then 
draw on the screen use that resource.
