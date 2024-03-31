#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class User {
private:
    std::string password;
protected:
    std::string username;
public:
    User() = default;
    User(const std::string& usern) : username(usern) {}
    User(const std::string& pass, const std::string& usern) : password(pass), username(usern) {}
    User(const User& other) : password(other.password), username(other.username) {}
    User& operator=(const User& other) {
        if (this != &other) {
            password = other.password;
            username = other.username;
        }
        return *this;
    }
    virtual ~User() = default;

    std::string getUsername() const { return username; }
    void setPassword(const std::string& pass) { password = pass; }

    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        os << "Username: " << user.username;
        return os;
    }
};

class Community {
private:
    std::unordered_map<std::string, std::vector<std::string>> userComments;
public:
    void getComments(const std::string& user) {
        for (const std::string& comm : userComments[user])
            std::cout << "User: " << user << "\n" << comm << '\n';
    }
};

class Channel {
private:
    int subCount;
    std::vector<std::string> videos;
    std::string channelName;
protected:
    std::shared_ptr<User> owner;
public:
    Channel(const std::string& channelName, std::shared_ptr<User> ownerPtr) : owner(ownerPtr), channelName(channelName), subCount(0) {}
    Channel(const Channel& other) : subCount(other.subCount), videos(other.videos), channelName(other.channelName), owner(other.owner) {}
    Channel& operator=(const Channel& other) {
        if (this != &other) {
            subCount = other.subCount;
            videos = other.videos;
            channelName = other.channelName;
            owner = other.owner;
        }
        return *this;
    }
    virtual ~Channel() = default;

    friend std::ostream& operator<<(std::ostream& os, const Channel& channel) {
        os << "Channel Name: " << channel.channelName << '\n';
        os << "Subscriber Count: " << channel.subCount << '\n';
        os << "Owner: " << *(channel.owner);
        return os;
    }

    void subscribe() {
        subCount++;
    }

    void unsubscribe() {
        if (subCount > 0)
            subCount--;
    }

    void publishVideo(const std::string& title) {
        videos.push_back(title);
    }

    void setChannelName(const std::string& ownerName) {
        if (channelName.empty()) {
            std::cout << ownerName << ", choose a name for the channel: ";
            std::cin >> channelName;
        }
    }
};

class MusicChannel : public Channel {
private:
    std::string musicLabel;
public:
    MusicChannel(const std::string& channelName, std::shared_ptr<User> ownerPtr) : Channel(channelName, ownerPtr), musicLabel("") {}
    MusicChannel(const MusicChannel& other) : Channel(other), musicLabel(other.musicLabel) {}
    MusicChannel& operator=(const MusicChannel& other) {
        if (this != &other) {
            Channel::operator=(other);
            musicLabel = other.musicLabel;
        }
        return *this;
    }
    std::string getLabel() const { return musicLabel; }
    void setLabel(const std::string& label) { if (musicLabel.empty()) musicLabel = label; }

    friend std::ostream& operator<<(std::ostream& os, const MusicChannel& channel) {
        os << static_cast<const Channel&>(channel) << '\n';
        os << "Music Label: " << channel.musicLabel << '\n';
        return os;
    }
};

class App {
private:
    std::vector<std::shared_ptr<User>> users; // Using shared_ptr for ownership
    std::vector<std::shared_ptr<Channel>> channels; // Using shared_ptr for ownership
public:
    ~App() = default;

    void addUser(std::shared_ptr<User> user) {
        users.push_back(user);
    }

    void addChannel(std::shared_ptr<Channel> channel) {
        channels.push_back(channel);
    }
};

int main() {
    App ytApp;
    std::shared_ptr<User> user1 = std::make_shared<User>("stefan");
    std::shared_ptr<User> user2 = std::make_shared<User>("dragonuak47");

    ytApp.addUser(user1);
    ytApp.addUser(user2);

    std::shared_ptr<Channel> channel1 = std::make_shared<Channel>("stefanpetre", user1);
    std::shared_ptr<Channel> channel2 = std::make_shared<MusicChannel>("Specii", user2);

    ytApp.addChannel(channel1);
    ytApp.addChannel(channel2);

    std::cout << "User Information:\n" << *user1 << "\n\n";
    std::cout << "Channel Information:\n" << *channel1 << "\n";

    channel1->subscribe();
    channel1->publishVideo("Rezolvari bac");
    channel1->publishVideo("Boomba");

    std::cout << "\n\ninfo:\n" << *channel1 << "\n";

    return 0;
}
