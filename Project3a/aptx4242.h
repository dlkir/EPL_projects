#if !(_aptx4242_h)
#define _aptx4242_h 1

#include "LifeForm.h"
#include "Init.h"

class aptx4242 : public LifeForm {
protected:
    int course_changed ;
    double perc_dist;
    int block[4];
    int last_block;
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void live(void);
    Event* hunt_event;
public:
    aptx4242(void);
    ~aptx4242(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static LifeForm* create(void);
    std::string player_name(void) const;
    std::string species_name(void) const;
    Action encounter(const ObjInfo&);
    friend class Initializer<aptx4242>;
    void which_block(double angle);
    int block_to_go(void);
    int at_which_block(double angle);
};


#endif /* !(_aptx4242_h) */
