/*
 * File:         SSH.hpp
 * Date:         January 2013
 * Description:  Class implementing the SSH protocol
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */


#ifndef SSH_HPP
#define SSH_HPP

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <libssh/callbacks.h>

#include <QtCore/QtCore>

class SSH
{
	
public:
  SSH();
  virtual ~SSH();
  
  char         mSSHError[256];
  
  void         CloseSSHSession();
  int          OpenSSHSession(QString IP, QString username, QString password);
  void         CloseSSHChannel();
  int          OpenSSHChannel();
  int          OpenSSHChannelWithPTY();
  void         CloseSFTPChannel();
  void         CloseAllSSH();
  int          ChannelRead(char * buffer, bool channel);
  int          ChannelWrite(const char * buffer);
  int          ReadRemotFile(const char * fileName, char * buffer);
  int          MakeRemoteDirectory(const char * directory);
  int          OpenSFTPChannel();
  int          verify_knownhost();
  int          SendFile(const char * source, const char * target);
  int          ExecuteSSHCommand(const char * command);
  int          ExecuteSSHGraphicCommand(const char * command, int commandSize);
  int          ExecuteSSHSudoCommand(const char * command, int commandSize, const char * password, int passwordSize);
  void         WaitEndSSHCommand();
  const char * getSSHError();

private:
  
  //***  SSH  ***//
  ssh_session  mSSHSession;
  ssh_channel  mSSHChannel;
  sftp_session mSFTPChannel;
  sftp_file    mSFTPFile; 
};

#endif
