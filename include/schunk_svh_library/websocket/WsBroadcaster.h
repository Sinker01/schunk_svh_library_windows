// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-


// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Georg Heppner <heppner@fzi.de>
 * \date    2014-5-20
 *
 */
//----------------------------------------------------------------------

#ifndef WSBROADCASTER_H
#define WSBROADCASTER_H

#include <schunk_svh_library/ImportExport.h>
#include <schunk_svh_library/websocket/ZMQClient.h>
#include <schunk_svh_library/websocket/WsbCallback.h>

#include <schunk_svh_library/websocket/RobotState.h>
#include <schunk_svh_library/websocket/LWA4PState.h>
#include <schunk_svh_library/websocket/SVHState.h>

#include <thread>
#include <chrono>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>


namespace schunk_svh_library {
namespace websocket {


//! definition of boost function callback for received JSON Message
typedef boost::function<void (const int &hint)> ReceivedHintCallback;


class DRIVER_SVH_IMPORT_EXPORT WsBroadcaster : WsbCallback
{
public:

  enum RobotType
  {
    eRT_LWA4P,
    eRT_SVH
  };

  WsBroadcaster(const RobotType &robot_type = eRT_LWA4P, const unsigned short recvPort=5566, unsigned short sendPort=5567)
    : m_socket(),
      m_recvPort(recvPort),
      m_sendPort(sendPort),
      m_send_error_counter(0),
      m_reset_error_counter(0)
  {
    switch (robot_type)
    {
      case eRT_SVH:
        robot = boost::shared_ptr<SVHState>(new SVHState());
        break;
      case eRT_LWA4P:
      default:
        robot = boost::shared_ptr<LWA4PState>(new LWA4PState());
        break;
    }
  }

  ~WsBroadcaster()
  {
    stopSimulation();
  }

  /*!
   * \brief registerMessageCallback register a function to call when the WS Broadcaster receives data
   * \param received_callback boost function signature of the function to call (with one string argument)
   */
  void registerHintCallback(ReceivedHintCallback const & received_callback)
  {
    m_received_callback = received_callback;
  }

  /*!
   * \brief checkSocket checks if the socket exists.If not, it is created and it adds callbacks to it and starts the listening process. If so nothing happens
   * \return true if it exists or everything worked out okay
   */
  bool checkSocket();

  /*!
   * \brief tryReconnect tries to reconnect to the socket
   */
  void tryReconnect();

  /*!
   * \brief sendHints Triggers the sending of all currently active hints via websocket
   * \return true if data could be sent
   */
  bool sendHints();

  /*!
   * \brief sendState Triggers an update of all the connected clients with new data
   * \return true if data could be sent
   */
  bool sendState();

  /*!
   * \brief simulateRobot Very simple thread to run the robot in simulation mode
   * \param cycle_time_ms cycle time for the thread. Every cycle_time_ms a step of the "simulation" is run
   */
  void simulateRobot(const int &cycle_time_ms)
  {
    while (true)
    {
      robot->simulateTick();
      std::this_thread::sleep_for(std::chrono::milliseconds(cycle_time_ms));
      sendState();
    }
  }

  /*!
   * \brief startSimulation Starts a thread to run the robot simulation
   * \param cycle_time_ms cycle time for the thread. Every cycle_time_ms a step of the "simulation" is run
   */
  void startSimulation(const int &cycle_time_ms)
  {
    robot->setTps(1000/cycle_time_ms);
    robot->setJointPositions(std::vector<double>(robot->getNumAxes(),0.0));
    m_simulation_thread = boost::thread(boost::bind(&WsBroadcaster::simulateRobot,this,cycle_time_ms));
  }

  /*!
   * \brief stopSimulation Stops the "simulation" of the robot
   */
  void stopSimulation()
  {
    m_simulation_thread.interrupt();
    m_simulation_thread.join();
  }


 // As the robot state and possible later states are just meant as a data storage for everyone to write their values to they are public for convenience

 //! Robot state representing the current state of the robot in terms of diagnostics
 boost::shared_ptr<RobotState> robot;

private:
 //! Unix Socket to communicate with the Websocket server
  boost::shared_ptr<ZMQClient> m_socket;
  unsigned short m_recvPort;
  unsigned short m_sendPort;
  size_t m_send_error_counter;
  size_t m_reset_error_counter;


 //! hread to simulate the robot output
 boost::thread m_simulation_thread;

 //! function callback for received hints (if other people are interested)
 ReceivedHintCallback m_received_callback;

 // WsbCallback interface
public:
 void onWSBClientMessage(std::string msg);
};

}} //NS end

#endif // WSBROADCASTER_H
