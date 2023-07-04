#include <templatebot/templatebot.h>
#include <sstream>
#include <string>
#include <random>


/* When you invite the bot, be sure to invite it with the
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */

const std::string bot_token = "Token";

int target;

int main(int argc, char const* argv[])
{
    /* Setup the bot */
    dpp::cluster bot(bot_token);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        if (event.command.get_command_name() == "greeting") {
            std::string name = std::get<std::string>(event.get_parameter("username"));
            event.reply(std::string("Hello ") + name);
        }
        if (event.command.get_command_name() == "add") {
            std::string integer1 = std::get<std::string>(event.get_parameter("number_1"));
            std::string integer2 = std::get<std::string>(event.get_parameter("number_2"));
            std::string ans = integer1 + " + " + integer2 + " = " + std::to_string(std::stoi(integer1) + std::stoi(integer2));
            event.reply(std::string("[Add] The result is ") + ans);
        }
        if (event.command.get_command_name() == "sub") {
            std::string integer1 = std::get<std::string>(event.get_parameter("number_1"));
            std::string integer2 = std::get<std::string>(event.get_parameter("number_2"));
            std::string ans = integer1 + " - " + integer2 + " = " + std::to_string(std::stoi(integer1) - std::stoi(integer2));
            event.reply(std::string("[Sub] The result is ") + ans);
        }
        if (event.command.get_command_name() == "mul") {
            std::string integer1 = std::get<std::string>(event.get_parameter("number_1"));
            std::string integer2 = std::get<std::string>(event.get_parameter("number_2"));
            std::string ans = integer1 + " * " + integer2 + " = " + std::to_string(std::stoi(integer1) * std::stoi(integer2));
            event.reply(std::string("[Mul] The result is ") + ans);
        }
        if (event.command.get_command_name() == "reset") {
            std::random_device rd;
            target = rd() % 100 + 1;
            event.reply("[Guess Number] Reset Successful!");
        }
        if (event.command.get_command_name() == "guess") {
            int guess = std::stoi(std::get<std::string>(event.get_parameter("number_guess")));
            if (guess == target) {
                event.reply("Bingo!");
            }
            else if (guess < target) {
                event.reply("Guess a larger number!");
            }
            else {
                event.reply("Guess a smaller number!");
            }
        }
        // custom command: roll the dice
        if (event.command.get_command_name() == "roll") {
            int quantity = std::stoi(std::get<std::string>(event.get_parameter("quantity")));
            int faces = std::stoi(std::get<std::string>(event.get_parameter("faces")));
            std::random_device rd;
            int sum = 0;
            std::string output = "";
            for (int i = 0; i < quantity; i++) {
                int tmp = rd() % faces + 1;
                sum += tmp;
                output += std::to_string(tmp);
                output += " ";
            }
            output += "] sum: " + std::to_string(sum);
            event.reply(std::string("[roll] The result is [") + output);
        }
        });

    /* Register slash command here in on_ready */
    bot.on_ready([&bot](const dpp::ready_t& event) {
        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
            dpp::slashcommand greeting("greeting", "Say Hello to the given username", bot.me.id);
            greeting.add_option(dpp::command_option(dpp::co_string, "username", "Please enter username", true));
            bot.global_command_create(greeting);
            dpp::slashcommand add("add", "add two integer", bot.me.id);
            add.add_option(dpp::command_option(dpp::co_string, "number_1", "Please enter an integer", true));
            add.add_option(dpp::command_option(dpp::co_string, "number_2", "Please enter an integer", true));
            bot.global_command_create(add);
            dpp::slashcommand sub("sub", "substract two integer", bot.me.id);
            sub.add_option(dpp::command_option(dpp::co_string, "number_1", "Please enter an integer", true));
            sub.add_option(dpp::command_option(dpp::co_string, "number_2", "Please enter an integer", true));
            bot.global_command_create(sub);
            dpp::slashcommand mul("mul", "multify two integer", bot.me.id);
            mul.add_option(dpp::command_option(dpp::co_string, "number_1", "Please enter an integer", true));
            mul.add_option(dpp::command_option(dpp::co_string, "number_2", "Please enter an integer", true));
            bot.global_command_create(mul);
            bot.global_command_create(dpp::slashcommand("reset", "Ramdomly generate an integer between 1 and 100", bot.me.id));
            dpp::slashcommand guess("guess", "Guess an integer between 1 and 100", bot.me.id);
            guess.add_option(dpp::command_option(dpp::co_string, "number_guess", "Please guess an integer between 1 and 100", true));
            bot.global_command_create(guess);

            // custom command: roll the dice 
            dpp::slashcommand roll("roll", "roll the dice with custom quantity and faces", bot.me.id);
            roll.add_option(dpp::command_option(dpp::co_string, "quantity", "Please enter an integer of the quantity of dice", true));
            roll.add_option(dpp::command_option(dpp::co_string, "faces", "Please enter an integer of the faces of dice", true));
            bot.global_command_create(roll);
        }
        });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}