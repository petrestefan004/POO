#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class User {
private:
    std::string password;
protected:
    std::string username;
public:
    User() = default;
    User(const std::string& usern) : username(usern) {}
    User(const std::string& pass, const std::string& usern) : password(pass), username(usern) {}
    virtual ~User() = default;

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
    User owner;
public:
    Channel(const std::string& channelName, const std::string& ownerName) : owner(ownerName), channelName(channelName), subCount(0) {}

    virtual ~Channel() = default;

    friend std::ostream& operator<<(std::ostream& os, const Channel& channel) {
        os << "Numele canalului: " << channel.channelName << '\n';
        os << "Numarul de abonati: " << channel.subCount << '\n';
        os << "Owner: " << channel.owner;
        return os;
    }
    Channel& operator=(const Channel& other) {
        if (this != &other) {
            channelName = other.channelName;
            subCount = 0;
            videos = {};
            owner = other.owner;
        }
        return *this;
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
            std::cout << ownerName << "Alege un nume pentru canal\n";
            std::cin >> channelName;
        }
    }
};

class MusicChannel : public Channel {
private:
    std::string musicLabel;
public:
    MusicChannel(const std::string& channelName, const std::string& ownerName) : Channel(channelName, ownerName), musicLabel("") {}

    void getLabel() {
        std::cout << "Music Label: " << musicLabel << '\n';
    }

    void setLabel(const std::string& label) {
        if (musicLabel.empty())
            musicLabel = label;
        else
            return;
    }
    MusicChannel& operator=(const MusicChannel& other) {
        if (this != &other) {
            Channel::operator=(other);
            musicLabel = other.musicLabel;
        }
        return *this;
    }
};

class App {
private:
    std::vector<User*> users;
    std::vector<Channel*> channels;
public:
    ~App() {
        for (auto user : users)
            delete user;
        for (auto channel : channels)
            delete channel;
    }

    void addUser(User* user) {
        users.push_back(user);
    }

    void addChannel(Channel* channel) {
        channels.push_back(channel);
    }
};

int main() {
    App ytApp;
    User* user1 = new User("stefan");
    User* user2 = new User("dragonuak47");

    ytApp.addUser(user1);
    ytApp.addUser(user2);

    Channel* channel1 = new Channel("stefanpetre", "eu");
    Channel* channel2 = new MusicChannel("Specii", "dragos");

    ytApp.addChannel(channel1);
    ytApp.addChannel(channel2);


    std::cout << "Informatia userului:\n" << *user1 << "\n\n";
    std::cout << "Informatia canalului:\n" << *channel1 << "\n";

    channel1->subscribe();
    channel1->publishVideo("Rezolvari bac");
    channel1->publishVideo("Boomba");

    std::cout << "\n\ntest:\n" << *channel1 << "\n";

    return 0;
}
