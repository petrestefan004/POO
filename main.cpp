#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <digestpp.hpp>

class PasswordManager {
public:
    static std::string make_salt() {
        static uint64_t nr = 1u;
        std::string salt;
        auto bytes = reinterpret_cast<const char*>(&nr);
        for(unsigned i = 0; i < 16; i++) {
            salt += bytes[i%8];
        }
        ++nr;
        return salt;
    }

    static std::string hash_password(const std::string& plain, const std::string& salt) {
        return digestpp::blake2b(512).set_salt(salt).absorb(plain).hexdigest();
    }
};

class User {
private:
    std::string password;
    std::string salt;
protected:
    std::string username;
public:
    User() = default;
    explicit User(std::string usern) : username(std::move(usern)) {}
    User(std::string pass, std::string usern, std::string sare) : password(std::move(pass)), salt(std::move(sare)) ,username(std::move(usern)){}
    User(const User& other) = default;
    User& operator=(const User& other) = default;
    virtual ~User() = default;

    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        os << "Username: " << user.username;
        return os;
    }

    [[maybe_unused]] [[nodiscard]] bool CheckLogin(const std::string& username_, const std::string& _password)const{
        std::string Hashedpassword = PasswordManager::hash_password(_password, salt);
        return (username==username_ && password==Hashedpassword);
    }
};

struct Video {
    std::string title;
    std::string link;
};

class Channel {
protected:
    int subCount;
    std::vector<Video> videos;
    std::string channelName;
    User* owner;
public:
    Channel(std::string channelName, User* ownerPtr) : subCount(0), videos(), channelName(std::move(channelName)), owner(ownerPtr) {}
    Channel(const Channel& other) = delete;
    Channel& operator=(const Channel& other) = delete;
    virtual ~Channel() = default;

    friend std::ostream& operator<<(std::ostream& os, const Channel& channel) {
        os << "Channel Name: " << channel.channelName << '\n';
        os << "Subscriber Count: " << channel.subCount << '\n';
        if (channel.owner) {
            os << "Owner: " << *(channel.owner) << '\n';
        }
        channel.printExtra(os);
        os << "Videos:\n";
        for (const auto& video : channel.videos) {
            os << "- " << video.title << " (" << video.link << ")\n";
        }
        return os;
    }

    void subscribe() {
        subCount++;
    }

    void unsubscribe() {
        if (subCount > 0)
            subCount--;
    }

    virtual void publishVideo(const std::string& title, const std::string& link) {
        videos.push_back({title, link});
    }

    virtual void displayChannelType() const = 0;

    virtual void printExtra(std::ostream& os) const = 0;

    [[nodiscard]] std::string getChannelName() const { return channelName; }
};

class GeneralChannel : public Channel {
public:
    GeneralChannel(const std::string& channelName, User* ownerPtr) : Channel(channelName, ownerPtr) {}

    void displayChannelType() const override {
        std::cout << "Channel Type: General\n";
    }

    void printExtra(std::ostream& os) const override {
        os << "Channel Type: General\n";
    }
};

class GamingChannel : public Channel {
private:
    std::string favoriteGame;
public:
    GamingChannel(const std::string& channelName, User* ownerPtr, std::string favoriteGame)
        : Channel(channelName, ownerPtr), favoriteGame(std::move(favoriteGame)) {}

    void displayChannelType() const override {
        std::cout << "Channel Type: Gaming\n";
        std::cout << "Favorite Game: " << favoriteGame << "\n";
    }

    void printExtra(std::ostream& os) const override {
        os << "Channel Type: Gaming\n";
        os << "Favorite Game: " << favoriteGame << "\n";
    }

    void publishVideo(const std::string& title, const std::string& link) override {
        Channel::publishVideo("[Gaming] " + title, link);
    }
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

    void displayChannelType() const override {
        std::cout << "Channel Type: Music\n";
    }

    void printExtra(std::ostream& os) const override {
        os << "Channel Type: Music\n";
        os << "Label: " << musicLabel << '\n';
    }

    void publishVideo(const std::string& title, const std::string& link) override {
        Channel::publishVideo("[Music] " + title, link);
    }

    void addSong(const std::string& song) {
        songs.push_back(song);
        publishVideo(song, "https://youtube.com/watch?v=" + song);
    }

    void addToPlaylist(const std::string& song) {
        playlist.push_back(song);
    }

    void markFavorite(const std::string& song) {
        auto it = std::find_if(playlist.begin(), playlist.end(), [&song](const std::string& s) {
            return s == song;
        });
        if (it != playlist.end()) {
            favorites.push_back(*it);
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
    static User* currentUser;
public:
    App()=default;

    App(const App& other)= default;

    App& operator=(const App& other)
    {
        if(this != &other)
        {
            users=other.users;
            channels=other.channels;
        }
        return *this;
    }

    [[maybe_unused]] App(const std::vector<User*>& _users, const std::vector<Channel*>& _channels)
        : users(_users), channels(_channels) {}

    ~App() {
        for (auto channel : channels) {
            delete channel;
        }
        for (auto user : users) {
            delete user;
        }
        std::cout<<"Delete App\n";
    }

     void signup()
    {
        std::cout<<"Welcome! Create a new account!\n";
        std::cout<<"Username: ";
        std::string username, password;
        std::cin>>username;
        std::cout<<"Password: ";
        std::cin>>password;
        std::string salt=PasswordManager::make_salt();
        std::string hashedPassword= PasswordManager::hash_password(password, salt);
        User *newuser= new User(hashedPassword, username, salt);
        users.push_back(newuser);
        std::cout << "Account created successfully.\n";
    }

    bool login(){
        std::cout<<"Welcome back! Please log in!\n";
        std::cout<<"Username: ";
        std::string username, password;
        std::cin>>username;
        std::cout<<"Password: ";
        std::cin>>password;

        auto it = std::find_if(users.begin(), users.end(), [&username, &password](const User* user) {
            return user->CheckLogin(username, password);
        });

        if (it != users.end()) {
            currentUser = *it;
            std::cout << "Login successful.\n";
            return true;
        }

        std::cout << "Invalid username or password.\n";
        return false;
    }

    void run() {
        int choice;
        while (true) {
            if (currentUser == nullptr) {
                std::cout << "\n=== YouTube App ===\n";
                std::cout << "1. Register\n";
                std::cout << "2. Login\n";
                std::cout << "3. Exit\n";
                std::cout << "Enter choice: ";
                if (!(std::cin >> choice)) break;

                if (choice == 1) {
                    signup();
                } else if (choice == 2) {
                    login();
                } else if (choice == 3) {
                    return;
                }
            } else {
                std::cout << "\n=== Dashboard ===\n";
                std::cout << "Welcome, " << *currentUser << "!\n";
                std::cout << "1. View All Channels\n";
                std::cout << "2. Exit\n";
                std::cout << "Enter choice: ";
                if (!(std::cin >> choice)) break;

                if (choice == 1) {
                    for (const auto channel : getChannels()) {
                        std::cout << "\nChannel Information:\n" << *channel << "\n";
                        channel->displayChannelType();
                    }
                } else if (choice == 2) {
                    break;
                }
            }
        }
    }

    void addUser(const std::string& username) {
        if (username.empty()) {
            std::cerr << "Error: Username cannot be empty." << std::endl;
            return;
        }
        users.push_back(new User(username));
    }

    void addChannel(Channel* channel) {
        channels.push_back(channel);
    }

    [[nodiscard]] User* getUser(size_t index) const {
        if (index < users.size()) {
            return users[index];
        }
        throw std::out_of_range("User index out of range");
    }

    [[nodiscard]] const std::vector<Channel*>& getChannels() const;
};
[[nodiscard]] const std::vector<Channel*>& App::getChannels() const {
    return channels;
}

User* App::currentUser = nullptr;

int main() {
    App ytApp;

    // Pre-populate some data for the user to interact with
    ytApp.addUser("stefan");
    ytApp.addUser("dragonuak47");
    ytApp.addUser("ionut");

    User* user1 = ytApp.getUser(0);
    User* user2 = ytApp.getUser(1);
    User* user3 = ytApp.getUser(2);

    ytApp.addChannel(new GeneralChannel("stefanpetre", user1));
    ytApp.addChannel(new GamingChannel("Specii", user2, "CS:GO"));

    MusicChannel* musicChannel = new MusicChannel("Luna_Amara", user3);
    musicChannel->setLabel("Independent_Music");
    std::cout << "Music Channel Label: " << musicChannel->getLabel() << "\n";

    musicChannel->addSong("Gri_Dorian");
    musicChannel->addSong("Rosu_Aprins");
    musicChannel->addSong("Dizident");

    musicChannel->addToPlaylist("Rosu_Aprins");
    musicChannel->addToPlaylist("Gri_Dorian");

    musicChannel->markFavorite("Rosu_Aprins");

    std::cout << "\nAll songs:\n";
    musicChannel->displaySongs();
    std::cout << "\nPlaylist:\n";
    musicChannel->displayPlaylist();
    std::cout << "\nFavorite songs:\n";
    musicChannel->displayFavorites();
    std::cout << "\n\n";

    ytApp.addChannel(musicChannel);

    auto channels = ytApp.getChannels();
    if (!channels.empty()) {
        Channel* firstChannel = channels[0];
        for (int i = 0; i < 5; ++i) {
            firstChannel->subscribe();
        }
        firstChannel->unsubscribe();
        firstChannel->publishVideo("Rezolvari_bac", "https://youtube.com/watch?v=12345");
        firstChannel->publishVideo("Boomba", "https://youtube.com/watch?v=67890");
    }

    // Run the interactive GUI loop
    ytApp.run();

    return 0;
}
