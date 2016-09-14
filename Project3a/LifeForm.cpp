/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;

//uint32_t LifeForm::_ID = 0; //for debug: ID

double time_tolerance = 0.001;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}

ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

//This is your movement event handler function. It calls update_position and then schedules the next movement event. You'll probably want to keep a pointer to the movement event as part of your LifeForm base class (that way, you can cancel this event, see below).
void LifeForm::border_cross(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::border_cross()"<<endl;//for debug
    std::cout<<"Event::now()"<<Event::now()<<endl;//for debug
    this->print();std::cout<<endl;//for debug
    
    update_position();
    check_encounter();
    if(speed!=0){compute_next_move();}
    return;
}

//this function will be a callback from the QuadTree. When another object is created nearby, your object needs to determine the next possible time that you could collide. The QuadTree knows when objects are inserted inside it, so it can invoke this callback function on your objects. What you will want to do is have region_resize cancel any pending border crossing events, update your position, and schedule the next movement event (since the region has changed size, you will encounter the boundary at a different time than than before.)
void LifeForm::region_resize(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::region_resize()"<<endl;//for debug
    std::cout<<"Event::now()"<<Event::now()<<endl;//for debug
    this->print();std::cout<<endl;//for debug
    
    border_cross_event->cancel();
    update_position();
    if(speed!=0.0){compute_next_move();}
    return;
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> that){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    if(that->is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::resolve_encounter(SmartPointer<LifeForm>)"<<endl;//for debug
    
    this->energy -= encounter_penalty;
    if(this->energy < min_energy){
        Action ActionThat = that->encounter(that->info_about_them(this));
        
        std::cout<<"=========================="<<this->species_name()<<" died of encounter "<<std::endl;//for debug
        this->die();
        
        return;
    }
    
    that->energy -= encounter_penalty;
    if(that->energy < min_energy){
        Action ActionThis = this->encounter(this->info_about_them(that));
        
        std::cout<<"=========================="<<that->species_name()<<" died of encounter "<<std::endl;//for debug
        space.remove(that->pos);
        std::cout<<"=========================="<<that->species_name()<<" removed from space"<<std::endl;//for debug
        that->is_alive = false;
        
        return;
    }
    
    ObjInfo this_sees_that=this->info_about_them(that);
    ObjInfo that_sees_this=that->info_about_them(this);
    
    Action ActionThis = this->encounter(this_sees_that);
    Action ActionThat = that->encounter(that_sees_this);
    
    bool this_eat_that = false;
    if(ActionThis==LIFEFORM_EAT && eat_success_chance(this->energy,that->energy) > drand48())
        this_eat_that = true;
    
    bool that_eat_this = false;
    if(ActionThat==LIFEFORM_EAT && eat_success_chance(that->energy,this->energy) > drand48())
        that_eat_this = true;
    
    if( this_eat_that && (!that_eat_this) ){
        this->eat(that);
    }else if( (!this_eat_that) && that_eat_this){
        that->eat(this);
    }else if( this_eat_that && that_eat_this ){
        if( that_sees_this.their_speed > this_sees_that.their_speed ){
            this->eat(that);
        }else if( that_sees_this.their_speed < this_sees_that.their_speed ){
            that->eat(this);
        }else if( this->energy > that->energy){
            this->eat(that);
        }else if( this->energy < that->energy ){
            that->eat(this);
        }else{//tie tie
            if(drand48()>0.5)
                this->eat(that);
        }
    }else{//neither success to eat
    }
    
    return;
}

void LifeForm::eat(SmartPointer<LifeForm> eatee){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    if(eatee->is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::eat(SmartPointer<LifeForm>)"<<endl;//for debug
    
    ObjInfo info_eatee = info_about_them(eatee);//for debug
    
    std::cout<<this->species_name()<<" is to eat "<<info_eatee.species<<std::endl;//for debug
    
    double to_gain = eat_efficiency*eatee->energy;
    
    
    
    std::cout<<"=========================="<<eatee->species_name()<<" died of being eaten "<<std::endl;//for debug
    space.remove(eatee->pos);
    std::cout<<"=========================="<<eatee->species_name()<<" removed from space"<<std::endl;//for debug
    eatee->is_alive = false;
    
    
    
    this->energy -= eat_cost_function(1.0,1.0);
    if(energy < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died when eating "<<info_eatee.species<<std::endl;//for debug
        this->die();
        return;
    }//eat cost function applied twice??????
    
    std::cout<<"~~~~~~~~~~~~~~~~Succesful Eat~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;//for debug
    std::cout<<this->species_name()<<" energy will be "<<this->energy+to_gain<<std::endl;//for debug
    std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;//for debug
    
    SmartPointer<LifeForm> self = SmartPointer<LifeForm>{this};
    if(is_alive==true){
        (void) new Event(digestion_time, [self,to_gain](void) {self->gain_energy(to_gain);});
    }
    return;
}

void LifeForm::age(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::age()"<<endl;//for debug
    energy -= age_penalty;
    if(energy < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died of aging "<<std::endl;//for debug
        this->die();
        return;
    }
    SmartPointer<LifeForm> self{ this };
    if(is_alive==true){
        (void) new Event(age_frequency, [self](void) { self->age(); });
    }
    return;
}

void LifeForm::gain_energy(double gain){
    std::cout<<"LifeForm::gain_energy()"<<endl;//for debug
    energy += gain;
    return;
}

//computes the new position, charge for energy consumed. To compute the new position, you'll need to know how long it has been since the last time you'd called update_position on this object (how much delta time has passed). You'll need a data member called update_time. Each time update_position is called, use Event::now() to see how much time has passed since update_time. Calculate the new position by multiplying your speed by the delta time. Don't forget to set update_time equal to Event::now() when you're done.
//@@ calculate the current position for an object.  If less than Time::tolerance time units have passed since the last call to update_position, then do nothing (we can't have moved very far so there's no point in updating our position)
void LifeForm::update_position(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::update_position()"<<endl;//for debug
    std::cout<<"Event::now()"<<Event::now()<<endl;//for debug
    this->print();std::cout<<endl;//for debug
    
    double dt = Event::now() - update_time;
    if(dt<time_tolerance){std::cout<<"don't update_position"<<endl;return;}
    Point new_pos;     //do we need smart pointer here??????
    new_pos.xpos = pos.xpos + speed*cos(course)*dt;
    new_pos.ypos = pos.ypos + speed*sin(course)*dt;
    
    if(space.is_out_of_bounds(new_pos)){
        std::cout<<"=========================="<<this->species_name()<<" died of falling off boundary "<<std::endl;//for debug
        this->die();
        return;
    }//fall off boundary, die
    
    std::cout<<"->("<<new_pos.xpos<<","<<new_pos.ypos<<")"<<endl;//for debug
    this->update_time = Event::now();
    
    this->energy -= movement_cost(speed,dt);
    if(this->energy < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died during moving "<<std::endl;//for debug
        this->die();
        return;
    }
    
    space.update_position(this->pos,new_pos);
    
    pos.xpos=new_pos.xpos;
    pos.ypos=new_pos.ypos;
    
    return;
}

//@@ check to see if there's another object within encounter_distance.  If there's an object nearby, invoke resove_encounter on ourself with the closest object
void LifeForm::check_encounter(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::check_encounter()"<<endl;//for debug
    
    SmartPointer<LifeForm> obj_encounter;
    double dmin = encounter_distance + 0.1;
    
    auto objs = space.nearby(this->pos,encounter_distance);//won't return itself
    
    for (auto iter = objs.begin(); iter < objs.end(); ++iter){
        //(*iter) is SmartPointer<LifeForm>
        if( (this->pos).distance((*iter)->pos) < dmin ){
            dmin = (this->pos).distance((*iter)->pos);
            obj_encounter = *iter ;
        }
    }
    
    if(dmin <= encounter_distance){//must have found one
        std::cout<<this->player_name()<<" encounters "<<obj_encounter->species_name()<<endl;//for debug
        resolve_encounter(obj_encounter);
    }
    
    return;
}

//@@ a simple function that creates the next border_cross_event
void LifeForm::compute_next_move(void){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::compute_next_move()"<<endl;//for debug
    std::cout<<"Event::now()"<<Event::now()<<endl;//for debug
    
    SmartPointer<LifeForm> self{this};
    if(is_alive==true){
        border_cross_event=new Event(space.distance_to_edge(pos,course)/speed+Point::tolerance, [self](void) { self->border_cross(); });
    }
    return;
}

//These functions should cancel any pending border_cross event, update the current position of the object and then schedule a new border_cross event based on the new course and speed. Note that an object that is stationary will never cross a border.
void LifeForm::set_course(double ang){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::set_course(double)"<<endl;//for debug
    border_cross_event->cancel();
    update_position();
    course = ang;
    if (speed!=0.0){compute_next_move();}
    return;
}

void LifeForm::set_speed(double spd){
    if(is_alive==false)return;//??????????????? not sure ????????????????
    
    std::cout<<"LifeForm::set_speed()"<<endl;//for debug
    border_cross_event->cancel();
    update_position();
    
    speed = spd;
    if(speed > max_speed)speed=max_speed;
    
    if (speed!=0.0){compute_next_move();}
    return;
}

//First, new is used to create a new LifeForm derived object. Then, reproduce in invoked on an existing object with the new object as the argument. The Reproduce method must insert the new object into the QuadTree and begin simulating it (see below for further game rules).
void LifeForm::reproduce(SmartPointer<LifeForm> child){
    
    std::cout<<"LifeForm::reproduce(SmartPointer<LifeForm>)"<<endl;//for debug
    this->print();std::cout<<" energy="<<this->energy<<" tries to reproduce "<<std::endl;//for debug
    
    assert(this->is_alive);
    
    update_position();
    
    double dt = Event::now() - this->reproduce_time;
    if(dt < min_reproduce_time){//don't set child->is_alive=true
        return;
    }
    
    SmartPointer<LifeForm> nearest;
    int attempt=0;
    do {attempt += 1;
        do{
            child->pos.ypos = this->pos.ypos + drand48() * reproduce_dist - reproduce_dist;
            child->pos.xpos = this->pos.xpos + drand48() * reproduce_dist - reproduce_dist;
        }while(space.is_out_of_bounds(child->pos));
        
        nearest = space.closest(child->pos);
        
    } while ((nearest && nearest->position().distance(child->position()) <= encounter_distance) && space.is_occupied(child->pos) && attempt< 6 );
    
    if(attempt==6){//don't set child->is_alive=true
        return;
    };
    
    child->start_point = child->pos;
    space.insert(child, child->pos, [child]() { child->region_resize(); });
    (void) new Event(age_frequency, [child](void) { child->age(); });
    
    child->is_alive = true;//Craig::Craig() calls Craig::start_up(),  which makes it move
//    child->ID=_ID;
//    _ID +=1;
    
    this->energy = this->energy*(1-reproduce_cost)/2;
    if(this->energy < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died during reproduce as parent"<<std::endl;//for debug
        
        this->die();
        
        
        std::cout<<"=========================="<<child->species_name()<<" died during reproduce as child"<<std::endl;//for debug
        space.remove(child->pos);
        std::cout<<"=========================="<<child->species_name()<<" removed from space"<<std::endl;//for debug
        child->is_alive=false;
        
        
        return;
    }
    child->energy = this->energy;
    
    this->reproduce_time=Event::now();
    child->reproduce_time=Event::now();
    
    std::cout<<"~~~~~~~~~~~~~~~~Succesful Reproduce~~~~~~~~~~~~~~~~~"<<std::endl;//for debug
    std::cout<<"Parent:";this->print();std::cout<<" energy="<<this->energy<<std::endl;//for debug
    std::cout<<"Child:"<<child->species_name()<<" energy="<<child->energy<<std::endl;//for debug
    std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;//for debug
    
    
    return;
}

ObjList LifeForm::perceive(double r){
    std::cout<<"LifeForm::perceive(double)"<<endl;//for debug
    
    ObjList List;//ObjList == std::vector<ObjInfo>;
    double radius;
    
//see if it's safe to update_position. Don't change anything!!!
    double dt = Event::now() - update_time;
    Point new_pos;     //do we need smart pointer here??????
    new_pos.xpos = pos.xpos + speed*cos(course)*dt;
    new_pos.ypos = pos.ypos + speed*sin(course)*dt;
    
    if(space.is_out_of_bounds(new_pos)){
        std::cout<<"=========================="<<this->species_name()<<" died of falling off boundary inside perceive"<<std::endl;//for debug
        this->die();
        return List;
    }//fall off boundary, die
    
    if((this->energy - movement_cost(speed,dt)) < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died during moving inside perceive"<<std::endl;//for debug
        this->die();
        return List;
    }
    
//actually update_position//////////////////////////
    update_position();
    
    if(r > max_perceive_range){
        radius = max_perceive_range;
    }else if(r < min_perceive_range){
        radius = min_perceive_range;
    }else{
        radius = r;
    }
    
    this->energy -= perceive_cost(radius);
    if(this->energy < min_energy){
        std::cout<<"=========================="<<this->species_name()<<" died of perceiving "<<std::endl;//for debug
        this->die();
        return List;
    }
    
    auto objs_0 = space.nearby(this->pos,radius);
    
    for (auto iter = objs_0.begin(); iter < objs_0.end(); ++iter){
        (*iter)->update_position();
    }
    
    auto objs = space.nearby(this->pos,radius);
    
    for (auto iter = objs.begin(); iter < objs.end(); ++iter){
        if((*iter)->is_alive==true)
            List.push_back(info_about_them(*iter));
    }
    
    return List;
}