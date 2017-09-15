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

# Create database
```SQL
create table min_bar(bar_time text, high real, open real, low real, close real);

with a as(
select substr(time, 0, 17)||':00.000' bar_time, min(time) min_time, max(time) max_time, min(bid) low, max(bid) high 
  from ticks 
group by bar_time
)
insert into min_bar(bar_time, high, open, low, close)
select a.bar_time, a.high, tmin.bid open, a.low, tmax.bid close
FROM a
inner join ticks tmin on tmin.time = a.min_time
inner join ticks tmax on tmax.time = a.max_time;

create unique index min_bar_time_index on min_bar(bar_time);

create table min5_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 0, 15) || substr('00'|| cast(substr(bar_time, 15, 2)/5*5 as text), -2) || ':00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into min5_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;


create unique index min5_bar_time_index on min5_bar(bar_time);

create table min15_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 0, 15) || substr('00'|| cast(substr(bar_time, 15, 2)/15*15 as text), -2) || ':00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into min15_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;


create unique index min15_bar_time_index on min15_bar(bar_time);

create table min30_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 0, 15) || substr('00'|| cast(substr(bar_time, 15, 2)/30*30 as text), -2) || ':00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into min30_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;


create unique index min30_bar_time_index on min30_bar(bar_time);

create table hour_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 1, 13)||':00:00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into hour_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;


create unique index hour_bar_time_index on hour_bar(bar_time);

create table hour4_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 1, 11) || substr('00'|| cast(substr(bar_time, 12, 2)/4*4 as text), -2) || ':00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into hour4_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;

create unique index hour4_bar_time_index on hour4_bar(bar_time);

create table day_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 1, 10)||' 00:00:00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into day_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;


create unique index day_bar_time_index on day_bar(bar_time);

create table month_bar(bar_time text, high real, open real, low real, close real);

with a as(
select  substr(bar_time, 1, 7)||'.01 00:00:00.000' new_bar_time 
        , min(bar_time) min_time, max(bar_time) max_time, min(low) low, max(high) high 
FROM min_bar
group by new_bar_time
)
insert into month_bar(bar_time, high, open, low, close)
select a.new_bar_time, a.high, tmin.open open, a.low, tmax.close close
FROM a
inner join min_bar tmin on tmin.bar_time = a.min_time
inner join min_bar tmax on tmax.bar_time = a.max_time;

create unique index month_bar_time_index on month_bar(bar_time);
```
