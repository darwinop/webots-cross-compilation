/*
 * File:         SSH.hpp
 * Date:         January 2013
 * Description:  Class implementing the SSH protocol
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 * TODO:         comment (fabien): the SSH class should not have any DARwIn-OP dependency
 *               -> SSH stuff is not reusable
 *               -> the code is difficult to understand because mixed
 */


#ifndef SSH_HPP
#define SSH_HPP

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <libssh/callbacks.h>

#include <QtCore/QtCore>

class SSH : public QObject {
  Q_OBJECT

  public:
                  SSH(QObject *parent);
    virtual      ~SSH();

    int           startRemoteCompilation(const QString &IPAddress, const QString &username, const QString &password, bool makeDefaultController);
    int           startRemoteControl(const QString &IPAddress, const QString &username, const QString &password);
    int           uninstall(const QString &IPAddress, const QString &username, const QString &password);
    void          terminate() { mTerminate = true; }
    const QString error();

  signals:
    void          print(const QString &message, bool error);
    void          status(const QString &message);
    void          done();

  protected:
    QString       mError;
    QString       mStdout;
    QString       mStderr;

  private:
    int           openSSHSession(const QString &IPAddress, const QString &username, const QString &password);
    void          closeSSHSession();
    int           openSSHChannel();
    int           openSFTPChannel();
    void          closeSSHChannel();
    void          closeSFTPChannel();
    int           readRemoteFile(const QString &fileName, char *buffer, int buffer_size);
    int           verifyKnownHost();
    int           sendFile(const QString &source, const QString &target);
    int           executeSSHCommand(const QString &command, bool display = true, bool wait = true);
    void          readChannel(bool display, int err);
    int           updateFrameworkIfNeeded();
    bool          isFrameworkUpToDate();
    int           updateFramework();
    int           updateWrapperIfNeeded(const QString &root_password);
    bool          isWrapperUpToDate();
    int           updateWrapper(const QString &root_password);

    ssh_session   mSSHSession;
    ssh_channel   mSSHChannel;
    sftp_session  mSFTPChannel;
    sftp_file     mSFTPFile;
    bool          mTerminate;
};

#endif
