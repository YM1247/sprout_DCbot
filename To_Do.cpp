#include <templatebot/templatebot.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <random>
#include <string.h>

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
            std::string name = std::get<std::string>(event.get_parameter("username"));//輸入名字
            event.reply(std::string("Hello ") + name);
        }
        if (event.command.get_command_name() == "add") {
            std::string integer1 = std::get<std::string>(event.get_parameter("number_1"));//輸入整數1(字串)
            std::string integer2 = std::get<std::string>(event.get_parameter("number_2"));//輸入整數2(字串)
            std::string ans = integer1 + " + " + integer2 + " = " + std::to_string(std::stoi(integer1) + std::stoi(integer2));//後面是把字串轉成整數再相加
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
        if (event.command.get_command_name() == "write"){
            dpp::interaction_modal_response modal("diary", "Please enter your diary");
            modal.add_component(
                dpp::component().
                set_label("DATE (IN FORMS OF YYYYMMDD)").
                set_id("field_id").
                set_type(dpp::cot_text).
                set_placeholder("YYYYMMDD").//日期
                set_min_length(1).
                set_max_length(50).
                set_text_style(dpp::text_short)
            );
            modal.add_row();
            modal.add_component(
                dpp::component().
                set_label("TITLE").
                set_id("寫個TITLE吧").
                set_type(dpp::cot_text).
                set_placeholder("寫個Title吧").//標題
                set_min_length(1).
                set_max_length(2000).
                set_text_style(dpp::text_short)
            );
            modal.add_row();
            modal.add_component(
                dpp::component().
                set_label("YOUR DIARY").
                set_id("寫一篇日記").
                set_type(dpp::cot_text).
                set_placeholder("寫篇日記吧").//內容
                set_min_length(1).
                set_max_length(2000).
                set_text_style(dpp::text_paragraph)
            );
            event.dialog(modal);
        }
         if (event.command.get_command_name() == "read") {
            std::string diary_date = std::get<std::string>(event.get_parameter("date"));
            std::string file_name = diary_date + ".txt";//找檔名是日期+.txt的檔案
            std::ifstream in;
            in.open(file_name);
            if (in.fail()){//失敗
                in.close();
                event.reply(std::string("Diary not found!!!!"));              
            }
            else{//成功
                std::string date, title, diary;
                getline(in, date);
                getline(in, title);
                while(!in.eof()){
                    std::string tmp;
                    in >> tmp;
                    diary += tmp + '\n';
                }
                in.close();
                dpp::embed embed = dpp::embed().
                    set_color(dpp::colors::sti_blue).
                    set_title(title).//標題
                    add_field(
                        "Date",//日期
                        date
                    ).
                    add_field(
                        "Content",//內容
                        diary,
                        true
                    ).
                    set_footer(dpp::embed_footer().set_text("My Diary at " + date)).
                    set_timestamp(time(0));
                dpp::message msg(bot.me.id, embed);//把embed塞成message
                event.reply(msg);
            }
        }
         if (event.command.get_command_name() == "remove") {
            std::string diary_date = std::get<std::string>(event.get_parameter("date"));
            std::string path = std::string(diary_date) + std::string(".txt");//找檔名是日期+.txt的檔案(相對路徑)
            if (std::remove(path.c_str()) == 0) {//成功
                event.reply("Diary deleted successfully :)");
            } 
            else {//失敗
                event.reply("Diary deletion failed :(");
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
            bot.global_command_create(dpp::slashcommand("write", "write your diary", bot.me.id));
            dpp::slashcommand read("read", "read your diary", bot.me.id );
            read.add_option(dpp::command_option(dpp::co_string, "date", "Please enter a date(YYYYMMDD)", true));
            bot.global_command_create(read);
            dpp::slashcommand remove("remove", "remove your diary", bot.me.id );
            remove.add_option(dpp::command_option(dpp::co_string, "date", "Please enter a date(YYYYMMDD)", true));
            bot.global_command_create(remove);

            // custom command: roll the dice 
            dpp::slashcommand roll("roll", "roll the dice with custom quantity and faces", bot.me.id);
            roll.add_option(dpp::command_option(dpp::co_string, "quantity", "Please enter an integer of the quantity of dice", true));
            roll.add_option(dpp::command_option(dpp::co_string, "faces", "Please enter an integer of the faces of dice", true));
            bot.global_command_create(roll);
        }
        });
    bot.on_form_submit([&bot](const dpp::form_submit_t & event) {
        std::string date = std::get<std::string>(event.components[0].components[0].value);//最上面的輸入框框
        std::string title = std::get<std::string>(event.components[1].components[0].value);//中間的輸入框框
        std::string diary = std::get<std::string>(event.components[2].components[0].value);//下面的輸入框框
        std::string file_name = date + ".txt";//檔名設成日期+.txt
        std::string input;
        dpp::message m;
        m.set_content("Date: " + date + '\n' + "Title: " + title + '\n' + "Content: " + diary).set_flags(dpp::m_ephemeral);
        input = date + '\n' + title + '\n' + diary;
        std::ofstream myfile;
        myfile.open(file_name);//創檔案
        myfile << input;
        myfile.close();
        event.reply(m);//輸出給使用者看他打了甚麼
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}
