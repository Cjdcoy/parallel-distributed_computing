#ifndef MY_ERRORS_H_
#define MY_ERRORS_H_

/** \addtogroup Utils
 *  @{
 */

#define EXIT				std::runtime_error("test")

#define ERROR_WRITE			std::runtime_error("write function failed")
#define ERROR_ARGUMENTS     std::runtime_error("Usage: blocking_udp_echo_client <host> <port>\n")
#define ERROR_INVALID_SIZE  std::runtime_error("Invalid size")
#define ERROR_LOAD_CFG      std::runtime_error("Couldn't open cfg file")
#define ERROR_LOAD_FILE     std::runtime_error("Error opening file")
#define ERROR_DELETING_FILE std::runtime_error("Error deleting file")
#define ERROR_FTOK          std::runtime_error("Generating key with ftok failed")
#define ERROR_SHMOPEN		std::runtime_error("Shmopen error")
#define ERROR_FTRUNCATE		std::runtime_error("Ftruncate error")
#define ERROR_SHM_WRITE		std::runtime_error("Parameters to write into the shared memory are invalid")
#define ERROR_MEMORY_ALLOC  std::bad_alloc()

/*
 * MASTER ERRORS
 */
#define ERROR_CHILD_RETRY   std::runtime_error("Too many errors coming from child")

#define ERROR_CHILD_DUP     std::runtime_error("Child couldn't dup standard input")
#define ERROR_CHILD_WRITE   std::runtime_error("Child couldn't write through pipe")

#define ERROR_MASTER_DUP    std::runtime_error("Master couldn't dup standard input")
#define ERROR_MASTER_WRITE  std::runtime_error("Master error reading standard input")
#define ERROR_MASTER_PIPE   std::runtime_error("Master couldn't create pipe")

/*
 * IPC ERRORS
 */
#define ERROR_SOCKET		std::runtime_error("Couldn't create socket")
#define ERROR_SETSOCKOPT	std::runtime_error("Setsockopt error")
#define ERROR_CONNECT		std::runtime_error("Couldn't connect to server")
#define ERROR_BIND			std::runtime_error("Enable to bind")
#define ERROR_NONBLOCK		std::runtime_error("Couldn't make socket non blocking")
#define ERROR_LISTEN		std::runtime_error("Listen failed")
#define ERROR_EPOLL_CREATE	std::runtime_error("Epoll create failed")
#define ERROR_EPOLL_CTL		std::runtime_error("Epoll ctl failed")
#define ERROR_FCNTL			std::runtime_error("Couldn't perfom fcntl on socket")
#define ERROR_PFUNTION_NFIND    std::runtime_error("Function name not found")
#define ERROR_CMD_FORM          std::runtime_error("Invalid command received")

/*
 * PARSER ERRORS
 */
#define ERROR_PARSER_FREQUENCY  std::runtime_error("Data Frequency error")
#define ERROR_PARSER_MODE       std::runtime_error("Error in GciResponseSnapshotV2::GciVectorizer::vectorize_into_file, mode value is not valid")
#define ERROR_FILL_BUFFER		std::runtime_error("invalid buffer identifier")

/** @}*/

#endif //MY_ERRORS_H_