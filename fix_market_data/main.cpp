#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

class FixApp : public FIX::Application, public FIX::MessageCracker
{
public:
    FixApp(const std::string username, const std::string pass_phrase) : username_(username), pass_phrase_(pass_phrase)
    {
    }
    void onCreate(const FIX::SessionID &) override {}
    void onLogon(const FIX::SessionID &) override
    {
        std::cout << "Logged in to Coinbase FIX Market Data\n";
    }
    void onLogout(const FIX::SessionID &) override
    {
        std::cout << "Logged out\n";
    }
    void toAdmin(FIX::Message &message, const FIX::SessionID &sessionID) override
    {
        FIX::MsgType msgType;
        message.getHeader().getField(msgType);
        if (msgType == FIX::MsgType_Logon)
        {
            message.setField(FIX::Username(username_));    // Tag 553
            message.setField(FIX::Password(pass_phrase_)); // Tag 554
        }
    }

    void toApp(FIX::Message &, const FIX::SessionID &) override {}
    void fromAdmin(const FIX::Message &message, const FIX::SessionID &) override
    {
        std::cout << "[ADMIN] " << message << "\n";
    }
    void fromApp(const FIX::Message &message, const FIX::SessionID &) override
    {
        std::cout << "[APP] " << message << "\n";
    }

private:
    std::string pass_phrase_;
    std::string username_;
};

std::atomic<bool> running{true};

void handleSignal(int)
{
    std::cout << "\nStopping...\n";
    running = false;
}

int main()
{
    std::signal(SIGINT, handleSignal);

    try
    {
        FIX::SessionSettings settings("quickfix.cfg");
        std::string username = "REPLACE_WITH_API_KEY";
        std::string passphrase = "REPLACE_WITH_API_PASSPHRASE";
        FixApp app{username, passphrase};
        FIX::FileStoreFactory storeFactory(settings);
        FIX::FileLogFactory logFactory(settings);
        FIX::SocketInitiator initiator(app, storeFactory, settings, logFactory);

        initiator.start();
        std::cout << "Connecting to Coinbase FIX MD... Press Ctrl+C to quit.\n";

        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        initiator.stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
