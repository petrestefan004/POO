#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>

class User {
private:
    std::string password;
protected:
    std::string username;
public:
    User() = default;
    explicit User(std::string usern) : username(std::move(usern)) {}
    User(std::string pass, std::string usern) : password(std::move(pass)), username(std::move(usern)) {}
    User(const User& other) = default;
    User& operator=(const User& other) = default;
    virtual ~User() = default;

    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        os << "Username: " << user.username;
        return os;
    }
};

class Channel {
private:
    int subCount;
    std::vector<std::string> videos;
    std::string channelName;
protected:
    User* owner;
public:
    Channel(std::string channelName, User* ownerPtr) : subCount(0), videos(), channelName(std::move(channelName)), owner(ownerPtr) {}
    Channel(const Channel& other) = delete;
    Channel& operator=(const Channel& other) = delete;
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

    [[nodiscard]] std::string getChannelName() const { return channelName; }
};

class MusicChannel : public Channel {
private:
    std::string musicLabel;
    std::vector<std::string> songs;
    std::vector<std::string> playlist;
    std::vector<std::string> favorites;
public:
    MusicChannel(const std::string& channelName, User* ownerPtr) : Channel(channelName, ownerPtr), songs(), playlist(), favorites() {}

    MusicChannel(const MusicChannel& other) = delete;
    MusicChannel& operator=(const MusicChannel& other) = delete;

    [[maybe_unused]] [[nodiscard]] std::string getLabel() const { return musicLabel; }

    [[maybe_unused]] void setLabel(const std::string& label) { musicLabel = label; }

    void addSong(const std::string& song) {
        songs.push_back(song);
    }

    void addToPlaylist(const std::string& song) {
        playlist.push_back(song);
    }

    void markFavorite(const std::string& song) {
        for (const auto& s : playlist) {
            if (s == song) {
                favorites.push_back(s);
                break;
            }
        }
    }


    void displaySongs() const {
        std::cout << "Songs in " << getChannelName() << ":\n";
        for (const auto& song : songs) {
            std::cout << song << '\n';
        }
    }

    void displayPlaylist() const {
        std::cout << "Playlist in " << getChannelName() << ":\n";
        for (const auto& song : playlist) {
            std::cout << song << '\n';
        }
    }

    void displayFavorites() const {
        std::cout << "Favorite songs in " << getChannelName() << ":\n";
        for (const auto& song : favorites) {
            std::cout << song << '\n';
        }
    }
};


class App {
private:
    std::vector<User*> users;
    std::vector<Channel*> channels;
public:

    ~App() {
        for (auto channel : channels) {
            delete channel;
        }
        for (auto user : users) {
            delete user;
        }
    }

    void addUser(const std::string& username) {
        if (username.empty()) {
            std::cerr << "Error: Username cannot be empty." << std::endl;
            return;
        }
        users.push_back(new User(username));
    }

    void addChannel(const std::string& channelName, const User& owner) {
        users.push_back(new User(owner));
        channels.push_back(new Channel(channelName, users.back()));
    }

    [[nodiscard]] const User& getUser(size_t index) const {
        if (index < users.size()) {
            return *users[index];
        }
        throw std::out_of_range("User index out of range");
    }

    [[nodiscard]] const std::vector<Channel*>& getChannels() const {
        return channels;
    }
};

int main() {
    App ytApp;

    ytApp.addUser("stefan");
    ytApp.addUser("dragonuak47");

    const User& user1 = ytApp.getUser(0);
    const User& user2 = ytApp.getUser(1);

    ytApp.addChannel("stefanpetre", user1);
    ytApp.addChannel("Specii", user2);

    std::cout << "User Information:\n" << user1 << "\n\n";
    for (const auto& channel : ytApp.getChannels()) {
        std::cout << "Channel Information:\n" << *channel << "\n\n";
    }

    auto channels = ytApp.getChannels();
    if (!channels.empty()) {
        Channel* firstChannel = channels[0];
        for (int i = 0; i < 5; ++i) {
            firstChannel->subscribe();
        }
        firstChannel->unsubscribe();
        firstChannel->publishVideo("Rezolvari_bac");
        firstChannel->publishVideo("Boomba");


        std::cout << "After Subscribing:\n" << *firstChannel << "\n\n";
    } else {
        std::cout << "No channels available.\n\n";
    }

    User owner("Ionut");
    MusicChannel musicChannel("Luna_Amara", &owner);

    musicChannel.addSong("Gri_Dorian");
    musicChannel.addSong("Rosu_Aprins");
    musicChannel.addSong("Dizident");

    musicChannel.addToPlaylist("Rosu_Aprins");
    musicChannel.addToPlaylist("Gri_Dorian");

    musicChannel.markFavorite("Dizident");

    std::cout << "All songs:\n";
    musicChannel.displaySongs();
    std::cout << "\nPlaylist:\n";
    musicChannel.displayPlaylist();
    std::cout << "\nFavorite songs:\n";
    musicChannel.displayFavorites();

    return 0;
}
