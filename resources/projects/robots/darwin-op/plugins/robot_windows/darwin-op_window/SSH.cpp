#include "SSH.hpp"

#include <stdlib.h>
#include <stdio.h> 
#include <fcntl.h>
#include <libtar.h>
#include <errno.h>
#include <string.h>

SSH::SSH() {
  mSSHSession = NULL;
  mSSHChannel = NULL;
  mSFTPChannel = NULL;
  mSFTPFile = NULL;
  strcpy(mSSHError, "No error");
}

SSH::~SSH() {
}

void SSH::CloseSSHSession() {
  if(mSSHSession != NULL) {
    ssh_disconnect(mSSHSession);
    ssh_free(mSSHSession);
    mSSHSession = NULL;
  }
}

int SSH::OpenSSHSession(QString IP, QString username, QString password) {
  // Open session and set options
  mSSHSession = ssh_new();
  if (mSSHSession == NULL) {
	  strcpy(mSSHError, ssh_get_error(mSSHSession));
      return -1;
  }

  ssh_options_set(mSSHSession, SSH_OPTIONS_HOST, (char*)IP.toStdString().c_str());
  ssh_options_set(mSSHSession, SSH_OPTIONS_USER, (char*)username.toStdString().c_str());

  // Connect to server
  if (ssh_connect(mSSHSession) != SSH_OK) {
    strcpy(mSSHError, ssh_get_error(mSSHSession));
    ssh_free(mSSHSession);
    mSSHSession = NULL;
    return -1;
  }

  // Verify the server's identity
  if (verify_knownhost() < 0) {
    strcpy(mSSHError, "Connection error : identity of the server not confirmed");
    ssh_disconnect(mSSHSession);
    ssh_free(mSSHSession);
    mSSHSession = NULL;
    return -1;
  }
  
  // Authenticate ourselves
  if (ssh_userauth_password(mSSHSession, NULL, (char*)password.toStdString().c_str()) != SSH_AUTH_SUCCESS) {
    strcpy(mSSHError, ssh_get_error(mSSHSession));
    ssh_disconnect(mSSHSession);
    ssh_free(mSSHSession);
    mSSHSession = NULL;
    return -1;
  }
  return 1;
}  


void SSH::CloseSSHChannel() {
  if(mSSHChannel != NULL) {
    ssh_channel_close(mSSHChannel);
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
  }
}

int SSH::OpenSSHChannel() {
  mSSHChannel = ssh_channel_new(mSSHSession);
  if (mSSHChannel == NULL)
    return -1;

  if (ssh_channel_open_session(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  return 1;
}  

int SSH::OpenSSHChannelWithPTY() {
  mSSHChannel = ssh_channel_new(mSSHSession);
  if (mSSHChannel == NULL)
    return -1;

  if (ssh_channel_open_session(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }

  if(ssh_channel_request_pty(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  
  if(ssh_channel_request_shell(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  return 1;
} 

int SSH::ExecuteSSHCommand(const char * command) {
  CloseSSHChannel();
  if(OpenSSHChannel() < 0)
    return -1;

  if (ssh_channel_request_exec(mSSHChannel, command) != SSH_OK) {
	strcpy(mSSHError, ssh_get_error(mSSHSession));
    return -1;
  }
  return 1;
}

int SSH::ExecuteSSHGraphicCommand(const char * command, int commandSize) {
  mSSHChannel = ssh_channel_new(mSSHSession);
  if (mSSHChannel == NULL)
    return -1;

  if (ssh_channel_open_session(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }

  if(ssh_channel_request_pty(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  if(ssh_channel_request_x11(mSSHChannel, 0, NULL, NULL, 0) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  
  if(ssh_channel_request_shell(mSSHChannel) != SSH_OK) {
    ssh_channel_free(mSSHChannel);
    mSSHChannel = NULL;
    return -1;
  }
  ssh_channel_write(mSSHChannel, "export DISPLAY=:0\n", sizeof("export DISPLAY=:0\n"));
  WaitEndSSHCommand();
  ssh_channel_write(mSSHChannel, command, commandSize);
  return 1;
}

int SSH::ExecuteSSHSudoCommand(const char * command, int commandSize, const char * password, int passwordSize) {
  if(OpenSSHChannelWithPTY() < 0)
    return -1;
  WaitEndSSHCommand();
  ssh_channel_write(mSSHChannel, "sudo su\n", sizeof("sudo su\n"));
  WaitEndSSHCommand();
  ssh_channel_write(mSSHChannel, password, passwordSize);
  WaitEndSSHCommand();
  ssh_channel_write(mSSHChannel, command, commandSize);
  WaitEndSSHCommand();
  return 1;
}

void SSH::CloseSFTPChannel() {
  if(mSFTPChannel != NULL) {
    sftp_free(mSFTPChannel);
    mSFTPChannel = NULL;
  }
}

int SSH::OpenSFTPChannel() {
  // Open SFTP channel
  mSFTPChannel = sftp_new(mSSHSession);
  if (mSFTPChannel == NULL) {
	strcpy(mSSHError, ssh_get_error(mSSHSession));
    return -1;
  }
    
  // Initilize SFTP channel
  if (sftp_init(mSFTPChannel) != SSH_OK) {
	strcpy(mSSHError, ssh_get_error(mSSHSession));
    return -1;
  }
  return 1;
}

int SSH::SendFile(const char * source, const char * target) {
  int i = 0;
  int access_type = O_WRONLY | O_CREAT | O_TRUNC, nwritten;
  
  // Open local file
  FILE * file;
  file = fopen(source, "rb"); 
  
  // Obtain file size:
  fseek(file , 0 , SEEK_END);
  int length = ftell(file);
  rewind(file);
  
  // Allocate memory to contain the whole file:
  char * buffer = (char*) malloc(sizeof(char)*length);
  
  // Copy the file into the buffer
  int size = fread(buffer,1,length,file);

  // Open remote file
#ifdef WIN32
  mSFTPFile = sftp_open(mSFTPChannel, target, access_type, 0);
#else
  mSFTPFile = sftp_open(mSFTPChannel, target, access_type, S_IRWXU);
#endif
  if (mSFTPFile == NULL) {
    strcpy(mSSHError, ssh_get_error(mSSHSession));
    fclose(file);
    free(buffer);
    return -1;
  }

  int maxPaquetSize = 200000;
  int packetNumber = (int)(size / maxPaquetSize);
  // Send packetNumber packet of 200kb
  for(i = 0; i < packetNumber; i++) {
    nwritten = sftp_write(mSFTPFile, (buffer + (i * maxPaquetSize)), maxPaquetSize);
    if (nwritten != maxPaquetSize) {
      strcpy(mSSHError, ssh_get_error(mSSHSession));
      sftp_close(mSFTPFile);
      fclose(file);
      free(buffer);
      return -1;
    }
  }
  // Send last packet smaller than 200kb
  nwritten = sftp_write(mSFTPFile, (buffer + (i * maxPaquetSize)), size - (packetNumber * maxPaquetSize));
  if (nwritten != (size - (packetNumber * maxPaquetSize))) {
    strcpy(mSSHError, ssh_get_error(mSSHSession));
    sftp_close(mSFTPFile);
    fclose(file);
    free(buffer);
    return -1;
  }
   
  // Close remote file
  if (sftp_close(mSFTPFile) != SSH_OK) {
    strcpy(mSSHError, ssh_get_error(mSSHSession));
    fclose(file);
    free(buffer);
    return -1;
  }

  // Close local file
  fclose(file);
  free(buffer);
  return 1;
}

void SSH::CloseAllSSH() {
  CloseSFTPChannel();
  CloseSSHChannel();
  CloseSSHSession();
}

void SSH::WaitEndSSHCommand() {
  // Wait end of command
  char console[512];
  while (ssh_channel_read(mSSHChannel, console, sizeof(console), 0) > 0) 
    {}
}

int SSH::verify_knownhost() {
  int state, hlen;
  unsigned char *hash = NULL;
  char *hexa;

  state = ssh_is_server_known(mSSHSession);
  hlen = ssh_get_pubkey_hash(mSSHSession, &hash);
  
  if (hlen < 0)
    return -1;
    
  switch (state) {
    case SSH_SERVER_KNOWN_OK:
      break; // ok 
    case SSH_SERVER_KNOWN_CHANGED:
      fprintf(stderr, "Host key for server changed: it is now:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      fprintf(stderr, "For security reasons, connection will be stopped\n");
      free(hash);
      return -1;
    case SSH_SERVER_FOUND_OTHER:
      fprintf(stderr, "The host key for this server was not found but an other type of key exists.\n");
      fprintf(stderr, "An attacker might change the default server key to confuse your client into thinking the key does not exist\n");
      free(hash);
      return -1;
    case SSH_SERVER_FILE_NOT_FOUND:
      //fprintf(stderr, "Could not find known host file.\n");
      //fprintf(stderr, "If you accept the host key here, the file will be automatically created.\n");
    // fallback to SSH_SERVER_NOT_KNOWN behavior
    case SSH_SERVER_NOT_KNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      //fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
      //fprintf(stderr, "Public key hash: %s\n", hexa);
      free(hexa);
      // This par is skipped to avoid user interaction
      //char buf[10];
      //if(fgets(buf, sizeof(buf), stdin) == NULL) {
      //  free(hash);
      //  return -1;
      //}
      //if(strncasecmp(buf, "yes", 3) != 0) {
      //  free(hash);
      //  return -1;
      //}
      if(ssh_write_knownhost(mSSHSession) < 0) {
        strcpy(mSSHError, strerror(errno));fprintf(stderr, "Error %s\n", strerror(errno));
        free(hash);
        return -1;
      }
      break;
    case SSH_SERVER_ERROR:
      strcpy(mSSHError, ssh_get_error(mSSHSession));//fprintf(stderr, "Error %s", ssh_get_error(mSSHSession));
      free(hash);
      return -1;
    }
    
  free(hash);
  return 0;
}

int SSH::ChannelRead(char * buffer, bool channel = false) {
  return ssh_channel_read(mSSHChannel, buffer, sizeof(buffer), channel); 
}

int SSH::MakeRemoteDirectory(const char * directory) {
#ifdef WIN32
  if(sftp_mkdir(mSFTPChannel, directory , 0) != 0)
#else
  if(sftp_mkdir(mSFTPChannel, directory , S_IRWXU) != 0)
#endif
    return -1;
  return 1;
}

int SSH::ReadRemotFile(const char * fileName, char * buffer) {
  sftp_file file;

  file = sftp_open(mSFTPChannel, fileName, O_RDONLY, 0);
  if (file == NULL) {
    return -1; // file not present
  }
  
  int size = sftp_read(file, buffer, sizeof(buffer));
  sftp_close(file);
  return size;
}

int SSH::ChannelWrite(const char * buffer) {
  return ssh_channel_write(mSSHChannel, buffer, sizeof(buffer));
}

const char * SSH::getSSHError() {
  return ssh_get_error(mSSHSession);
}
