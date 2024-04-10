#include <iostream>
#include <utility>
#include <vector>
#include <string>

class User {
private:
    std::string password;
protected:
    std::string username;
public:
    User() =default;
    explicit User(std::string  usern) :  username(std::move(usern)) {}
    User(std::string  pass, std::string  usern) : password(std::move(pass)), username(std::move(usern)) {}
    User(const User& other)  = default;
    User& operator=(const User& other) {
        if (this != &other) {
            password = other.password;
            username = other.username;
        }
        return *this;
    }
    virtual ~User() = default;

    ///[[nodiscard]] std::string getUsername() const { return username; }
   /// [[nodiscard]] std::string getPassword() const { return password; }

    ///void setPassword(const std::string& pass) { password = pass; }

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
    Channel(std::string  channelName, User* ownerPtr) : subCount(0), videos(), channelName(std::move(channelName)), owner(ownerPtr) {}
    Channel(const Channel& other) : subCount(other.subCount), videos(other.videos), channelName(other.channelName), owner(new User(*other.owner)) {}
    Channel& operator=(const Channel& other) {
        if (this != &other) {
            subCount = other.subCount;
            videos = other.videos;
            channelName = other.channelName;
            delete owner;
            owner = new User(*other.owner);
        }
        return *this;
    }
    virtual ~Channel() {
        delete owner;
    }

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
    MusicChannel(const std::string& channelName, User* ownerPtr) : Channel(channelName, ownerPtr),  songs(), playlist(), favorites() {}

    MusicChannel(const MusicChannel& other) = default;

    MusicChannel& operator=(const MusicChannel& other) {
        if (this != &other) {
            Channel::operator=(other);
            musicLabel = other.musicLabel;
            songs = other.songs;
            playlist = other.playlist;
            favorites = other.favorites;
        }
        return *this;
    }

    [[maybe_unused]] [[nodiscard]] std::string getLabel() const { return musicLabel; }

    [[maybe_unused]] void setLabel(const std::string& label) { musicLabel = label; }

    // Add a song to the channel
    void addSong(const std::string& song) {
        songs.push_back(song);
    }

    // Add a song to the playlist
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
    std::vector<User *> users;
    std::vector<Channel *> channels;
public:
    ~App() {
        for (auto user: users)
            delete user;
        for (auto channel: channels)
            delete channel;
    }

    void addUser(const std::string &username) {
        users.push_back(new User(username));
    }

    void addChannel(const std::string &channelName, User *owner) {
        channels.push_back(new Channel(channelName, owner));
    }

    [[nodiscard]] User *getUser(size_t index) const {
        if (index < users.size())
            return users[index];
        return nullptr;
    }


    [[maybe_unused]] [[nodiscard]] const std::vector<User *> &getUsers() const {
        return users;
    }


    [[nodiscard]] const std::vector<Channel *> &getChannels() const {
        return channels;
    }
};
int main() {
    App ytApp;

    ytApp.addUser("stefan");
    ytApp.addUser("dragonuak47");

    User* user1 = ytApp.getUser(0);
    User* user2 = ytApp.getUser(1);

    ytApp.addChannel("stefanpetre", user1);
    ytApp.addChannel("Specii", user2);

    std::cout << "User Information:\n" << *user1 << "\n\n";
    std::cout << "Channel Information:\n" << *(ytApp.getChannels()[0]) << "\n";

    for(int i=0;i<5;i++)
        ytApp.getChannels()[0]->subscribe();
    ytApp.getChannels()[0]->unsubscribe();
    ytApp.getChannels()[0]->publishVideo("Rezolvari bac");
    ytApp.getChannels()[0]->publishVideo("Boomba");

    std::cout << "\n\nAfter Subscribing:\n" << *(ytApp.getChannels()[0]) << "\n";


    User owner("Ionut");
    MusicChannel musicChannel("Luna Amara", &owner);

    musicChannel.addSong("Gri Dorian");
    musicChannel.addSong("Rosu Aprins");
    musicChannel.addSong("Dizident");

    musicChannel.addToPlaylist("Rosu Aprins");
    musicChannel.addToPlaylist("Gri Dorian");

    musicChannel.markFavorite("Dizident");

    std::cout << "All songs:\n";
    musicChannel.displaySongs();

    std::cout << "\nPlaylist:\n";
    musicChannel.displayPlaylist();

    std::cout << "\nFavorite songs:\n";
    musicChannel.displayFavorites();

    return 0;
}
