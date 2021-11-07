/***************************************************************/
/*                                                             */
/* lizard.cpp                                                  */
/*                                                             */
/* To compile, you need all the files listed below             */
/*   lizard.cpp                                                */
/*                                                             */
/* Be sure to use the -lpthread option for the compile command */
/*   g++ -g -Wall -std=c++11 lizard.cpp -o lizard -pthread     */
/*                                                             */
/* Execute with the -d command-line option to enable debugging */
/* output.  For example,                                       */
/*   ./lizard -d                                               */
/*                                                             */
/***************************************************************/

/* FILE EDITED BY JOHN BUTLER AND BEA ALMONTE */

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <semaphore.h> // JB BA



using namespace std;

/*
 * This is a stub file.  It contains very little code and what
 * it does contain may need to be altered or removed.  It is
 * only provided for a starting point.
 *
 * The comments are probably useful.
 */


/*
 * Define "constant" values here
 */

/*
 * Make this 1 to check for lizards travelling in both directions
 * Leave it 0 to allow bidirectional travel
 */
#define UNIDIRECTIONAL       0

/*
 * Set this to the number of seconds you want the lizard world to
 * be simulated.  
 * Try 30 for development and 120 for more thorough testing.
 */
#define WORLDEND             180 // JB BA

/*
 * Number of lizard threads to create
 */
#define NUM_LIZARDS          20

/*
 * Number of cat threads to create
 */
#define NUM_CATS             2

/*	
 * Maximum lizards crossing at once before alerting cats
 */
#define MAX_LIZARD_CROSSING  4

/*
 * Maximum seconds for a lizard to sleep
 */
#define MAX_LIZARD_SLEEP     3

/*
 * Maximum seconds for a cat to sleep
 */
#define MAX_CAT_SLEEP        3

/*
 * Maximum seconds for a lizard to eat
 */
#define MAX_LIZARD_EAT       5

/*
 * Number of seconds it takes to cross the driveway
 */
#define CROSS_SECONDS        2


/*
 * Declare global variables here
 */
sem_t road; // mutex for semaphore -- JB BA
mutex step; // lock to step in or out -- JB BA
int lizardsOnRdCnt; // number of lizards on road -- JB BA

/**************************************************/
/* Please leave these variables alone.  They are  */
/* used to check the proper functioning of your   */
/* program.  They should only be used in the code */
/* I have provided.                               */
/**************************************************/
int numCrossingSago2MonkeyGrass;
int numCrossingMonkeyGrass2Sago;
int debug;
int running;
/**************************************************/



/**
 * This class models a cat that sleep, wakes-up, checks on lizards in the driveway
 * and goes back to sleep. If the cat sees enough lizards it "plays" with them.
 */
class Cat {
	
	int       _id;        // the Id of the cat
	thread*   _catThread; // the thread simulating the cat
	
	public:
		Cat (int id);
		int getId();
		void run();
		void wait();
    
    private:
		void sleepNow();

        static void catThread (Cat *aCat); 
};

/**
 * Constructs a cat.
 *
 * @param id - the Id of the cat 
 */
Cat::Cat (int id)
{
	_id = id;
}

/**
 * Returns the Id of the cat.
 *
 * @return the Id of a cat
 */
int Cat::getId()
{
	return _id;
}


/**
 * Launches a cat thread.
 * 
 * Status: Incomplete - Make changes to this code.
 */
 void Cat::run() 
 {
	 // launch the thread to simulate the cat's behavior	 
	// calls CAT THREAD and passes self
    _catThread = new thread ( catThread, this); // JB BA
    
 }
 
 /**
  * Waits for a cat to finish running.
  *
  * Status: Incomplete - Make changes to this code.
  */
 void Cat::wait()
 {
	// wait for the thread to terminate
    // deletes thread variable after joining -- JB BA
    if (_catThread != NULL) {   // JB BA
        _catThread->join();     // JB BA
        delete _catThread;      // JB BA
    }
    
 }
 
 
/**
 * Simulate a cat sleeping for a random amount of time
 *
 *
 * @return N/A
 *
 * Status: Completed - No need to change any of this code.
 */
void Cat::sleepNow()
{
	int sleepSeconds;

    // combined output string
    string output;  // JB BA

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_CAT_SLEEP);

	if (debug)
    {
		// changed to have outputs not jumbled
        output = "[" + to_string(_id) + "] cat sleeping for " + to_string(sleepSeconds) + " seconds\n"; // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
        // changed to have outputs not jumbled
        output = "[" + to_string(_id) + "] cat awake\n";    // JB BA
        cout << output;     // JB BA
		cout << flush;
    }
}

/**
  *
  * This simulates a cat that is sleeping and occasionally checking on
  * the driveway on lizards.
  * 
  * @param aCat - a cat that is being run concurrently
  * 
  * @return - N/A
  *
  * Status: Incomplete - Make changes as you see are necessary.
  */
void Cat::catThread (Cat *aCat)
{
	
	if (debug)
    {
		cout << "[" << aCat->getId() << "] cat is alive\n";
		cout << flush;
    }

	while(running)
    {
		aCat->sleepNow();

		/*
	     * Check for too many lizards crossing
	     */
        // locks the checking of lizards to one cat
        step.lock();    // JB BA
		if (numCrossingSago2MonkeyGrass + numCrossingMonkeyGrass2Sago > MAX_LIZARD_CROSSING)
		{
		  cout << "\tThe cats are happy - they have toys.\n";
          std::cout << std::flush;
		  exit( -1 );
		}
        step.unlock();  // JB BA
    }
}

 

class Lizard {
	int     _id;      // the Id of the lizard
	thread* _aLizard; // the thread simulating the lizard

	public:
		Lizard(int id);
		int getId();
        void run();
        void wait();

    private:
		void sago2MonkeyGrassIsSafe();
		void crossSago2MonkeyGrass();
		void madeIt2MonkeyGrass();
		void eat();
		void monkeyGrass2SagoIsSafe();
		void crossMonkeyGrass2Sago();
		void madeIt2Sago();
		void sleepNow();

        static void lizardThread(Lizard *aLizard); 
	
};


/**
 * Constructs a lizard.
 *
 * @param id - the Id of the lizard 
 */
Lizard::Lizard (int id)
{
	_id = id;
}

/**
 * Returns the Id of the lizard.
 *
 * @return the Id of a lizard
 */
int Lizard::getId()
{
	return _id;
}

/**
 * Launches a lizard thread.
 * 
 * Status: Incomplete - Make changes to this code.
 */
 void Lizard::run() 
 {
	 // launch the thread to simulate the lizard's behavior
    _aLizard = new thread ( lizardThread, this); // calls lizardthread and passes self
 }
 
 /**
  * Waits for a lizard to finish running.
  *
  * Status: Incomplete - Make changes to this code.
  */
 void Lizard::wait()
 {
	 // wait for the thread to terminate
	if (_aLizard != NULL) {
        _aLizard->join();
        // deletes thread variable after joining
        delete _aLizard;    // JB BA
    }
 }
 





/**
 * Simulate a lizard sleeping for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::sleepNow()
{
	int sleepSeconds;

    // combined output string
    string output;  // JB BA

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_SLEEP);

	if (debug)
    {
      output = "[" + to_string(_id) + "] sleeping for " + to_string(sleepSeconds) + " seconds\n";   // JB BA
      cout << output;   // JB BA
      cout << flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
      output = "[" + to_string(_id) + "] awake\n";  // JB BA
      cout << output;   // JB BA
      cout << flush;
    }
}



 
/**
 *
 * Returns when it is safe for this lizard to cross from the sago
 * to the monkey grass.   Should use some synchronization 
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::sago2MonkeyGrassIsSafe()
{
    // combined output string
    string output;  // JB BA
	if (debug)
    {
        output = "[" + to_string(_id) + "] checking sago -> monkey grass\n";    // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

    // uses resource, blocked until a resource is freed
    sem_wait(&road);    // JB BA


	if (debug)
    {
        output = "[" + to_string(_id) + "] thinks  sago -> monkey grass  is safe\n";    // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

    // crossing for number of seconds specified
    crossSago2MonkeyGrass();    // JB BA
     // releases semaphore resource then eats
    madeIt2MonkeyGrass();       // JB BA
}


/**
 *
 * Delays for 1 second to simulate crossing from the sago to
 * the monkey grass. 
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossSago2MonkeyGrass()
{
    string output;  // JB BA
	if (debug)
    {
      output = "[" + to_string(_id) + "] crossing sago -> monkey grass\n";  // JB BA
      cout << output;   // JB BA
      cout << flush;
    }

	/*
	 * One more crossing this way
	 */
    step.lock();    // JB BA
	numCrossingSago2MonkeyGrass++;
    // used for determining correct number of lizards on roadway JB BA
    // cerr << numCrossingMonkeyGrass2Sago + numCrossingSago2MonkeyGrass; JB BA
    step.unlock();  // JB BA
	/*
     * Check for lizards cross both ways
     */
	if (numCrossingMonkeyGrass2Sago && UNIDIRECTIONAL)
    {
		cout << "\tCrash!  We have a pile-up on the concrete." << endl;
		cout << "\t" << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t" << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }


	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

    /*
     * That one seems to have made it
     */
    step.lock(); //JB BA
    numCrossingSago2MonkeyGrass--;
    // used for determining correct number of lizards on roadway JB BA
    // cerr << numCrossingMonkeyGrass2Sago + numCrossingSago2MonkeyGrass; JB BA
    step.unlock(); // JB BA
}


/**
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2MonkeyGrass()
{
    string output;  // JB BA
	/*
     * Whew, made it across
     */
	if (debug)
    {
        output = "[" + to_string(_id) + "] made the  sago -> monkey grass  crossing\n"; // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

    // release resource for other threads
    sem_post(&road); // JB BA
    eat();  // eat for some random time


}


/**
 * Simulate a lizard eating for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::eat()
{
	int eatSeconds;
    string output;  // JB BA

	eatSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_EAT);

	if (debug)
    {
        output = "[" + to_string(_id) + "] eating for " + to_string(eatSeconds) + " seconds\n";     // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

	/*
     * Simulate eating by blocking for a few seconds
     */
	sleep( eatSeconds );

	if (debug)
    {
      output = "[" + to_string(_id) + "] finished eating\n";    // JB BA
      cout << output;       // JB BA
      cout << flush;
    }
}


/**
 * Returns when it is safe for this lizard to cross from the monkey
 * grass to the sago.   Should use some synchronization 
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::monkeyGrass2SagoIsSafe()
{
    string output;  // JB BA
	if (debug)
    {
        output = "[" + to_string(_id) + "] checking  monkey grass -> sago\n";   // JB BA
        cout << output; // JB BA
		cout << flush;
    }

    // takes semaphore resource, blocks until resource if freed
    sem_wait(&road);    // JB BA


	if (debug)
    {
        output = "[" + to_string(_id) + "] thinks  monkey grass -> sago  is safe\n";    // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

    // cross driveway
    crossMonkeyGrass2Sago();    // JB BA
    // release semaphore resource, sleep for random seconds
    madeIt2Sago();  // JB BA
}



/**
 * Delays for 1 second to simulate crossing from the monkey
 * grass to the sago. 
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossMonkeyGrass2Sago()
{
    string output = "";     // JB BA
	if (debug)
    {
        output = "[" + to_string(_id) + "] crossing  monkey grass -> sago\n";   // JB BA
        cout << output;     // JB BA
		cout << flush;
    }

    /*
     * One more crossing this way
     */
    step.lock();    // JB BA
	numCrossingMonkeyGrass2Sago++;
    // used for determining correct number of lizards on roadway JB BA
    // cerr << numCrossingMonkeyGrass2Sago + numCrossingSago2MonkeyGrass; JB BA
    step.unlock();  // JB BA
  
    /*
     * Check for lizards cross both ways
     */
	if (numCrossingSago2MonkeyGrass && UNIDIRECTIONAL)
    {
		cout << "\tOh No!, the lizards have cats all over them." << endl;
		cout << "\t " << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t " << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }

	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

	/*
     * That one seems to have made it
     */
    step.lock();    // JB BA
	numCrossingMonkeyGrass2Sago--;
    // used for determining correct number of lizards on roadway JB BA
    // cerr << numCrossingMonkeyGrass2Sago + numCrossingSago2MonkeyGrass; JB BA
    step.unlock();  // JB BA
}


/**
 *
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2Sago()
{
	/*
     * Whew, made it across
     */
  string output = "";   // JB BA

	if (debug)
    {
		output = "[" + to_string(_id) + "] made the  monkey grass -> sago  crossing\n"; // JB BA
		cout << output; // JB BA
		cout << flush;
    }

    // release semaphore for other threads
    sem_post(&road);    // JB BA
    sleepNow(); // sleep for some random time
}

/**
  * Follows the algorithm provided in the assignment
  * description to simulate lizards crossing back and forth
  * between a sago palm and some monkey grass. 
  *  
  * @param aLizard - the lizard to be executed concurrently
  *
  * @return N/A
  
  * Status: Incomplete - Make changes as you see are necessary.
  */
void Lizard::lizardThread(Lizard *aLizard)
{	
	string output;  // JB BA

	if (debug)
    {
      output = "[" + to_string(aLizard->getId()) + "] lizard is alive\n";   // JB BA
      cout << flush;
    }

	while(running)
    {
      /* 
       * Follow the algorithm given in the assignment
       * using calls to the functions declared above.
       * You'll need to complete the implementation of
       * some functions by filling in the code.  Some  
       * are already completed - see the comments.
       */
		//int random = rand() % MAX_LIZARD_SLEEP; // jb ba
        //std::cout << random << std::endl;
      	//sleep(random); // jb ba

        // sleep for some time
        aLizard->sleepNow();// JB BA
        // crossing to monkey grass
        // 1. checks if safe (not blocked by semaphore)
        // 2. increments # of lizards on road
        // 3. cross
        // 4. release semaphore
        // 5. eat
        aLizard->sago2MonkeyGrassIsSafe(); // JB BA
        // crossing to sago
        // 1. checks if safe (not blocked by semaphore)
        // 2. increments # of lizards on road
        // 3. cross
        // 4. release semaphore
        // 5. sleep
        aLizard->monkeyGrass2SagoIsSafe(); // JB BA

		}

}
 



/*
 * main()
 *
 * Should initialize variables, locks, semaphores, etc.
 * Should start the cat thread and the lizard threads
 * Should block until all threads have terminated
 * Status: Incomplete - Make changes to this code.
 */
int main(int argc, char **argv)
{
	/*
	 * Declare local variables
     */




	/*
     * Check for the debugging flag (-d)
     */
	debug = 0;
	if (argc > 1)
		if (strncmp(argv[1], "-d", 2) == 0)
			debug = 1;


	/*
     * Initialize variables
     */
	numCrossingSago2MonkeyGrass = 0;
	numCrossingMonkeyGrass2Sago = 0;
	running = 1;


	/*
     * Initialize random number generator
	 */
	srandom( (unsigned int)time(NULL) );


	/*
     * Initialize locks and/or semaphores
     */
    // unnamed
    sem_init(&road,0,MAX_LIZARD_CROSSING);  // JB BA

	/*
     * Create NUM_LIZARDS lizard threads
     */
    vector<Lizard*> allLizards;
    for (int i=0; i < NUM_LIZARDS; i++) {
	    allLizards.push_back(new Lizard(i));
    }    
    

    /*
     * Create NUM_CATS cat threads
     */
    // JB BA 
    // create Cat object vector
	  vector<Cat*> allCats;     // JB BA
	  for (int i = 0; i < NUM_CATS; i++)    // JB BA
	  {
		  allCats.push_back(new Cat(i));    // JB BA
	  }

	/*
	 * Run NUM_LIZARDS and NUM_CATS threads
	 */
    for (int i=0; i < NUM_LIZARDS; i++) {
        allLizards[i]->run();
    }
    for (int i = 0; i < NUM_CATS; i++)
	{
		  allCats[i]->run();    // JB BA
	}

	/*
     * Now let the world run for a while
     */
	sleep( WORLDEND );


	/*
     * That's it - the end of the world
     */
	running = 0;


    /*
     * Wait until all threads terminate
     */

    for (int i=0; i < NUM_LIZARDS; i++)    // JB BA
    {   
        allLizards[i]->wait();  // JB BA
    }
    for (int i = 0; i < NUM_CATS; i++)  // JB BA
	{
		  allCats[i]->wait();   // JB BA
	}


	/*
     * Delete the locks and semaphores
     */
	sem_destroy(&road);     // JB BA
	 
	/*
	 * Delete all cat and lizard objects
	 */
    for (int i=0; i < NUM_LIZARDS; i++)     // JB BA
    {
        delete allLizards[i];   // JB BA
    }
    for (int i = 0; i < NUM_CATS; i++)  // JB BA
	{
	    delete allCats[i];  // JB BA
	}



	/*
     * Exit happily
     */
	return 0;
}






