import os
import re
import sys

import pathlib
import subprocess
import git
import argparse

__hmtl_str_begin__ = """
<!DOCTYPE html>
<html lang="zn-CH">
<head>
	<meta charset="utf-8"/>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>更新日志</title>
</head>
<body>
<ul>

        """

end_str = """
</ul>

</body>
</html>
        """
__hmtl_str_end__ = """
</ul>

</body>
</html>
        """


class comm_mess():
    def __init__(self, in_mess: str, in_hax, in_date_time):
        self.mess = in_mess
        self.hax = in_hax
        self.time = in_date_time

    def __str__(self):
        return "{} {} {}".format(self.hax, self.mess, self.time)


class git_log():
    def __call__(self, *args, **kwargs):
        self.mess_reg = re.compile("""log|doc|\\.\\.\\.|\\.\\.|(\\d\\.\\d\\.\\d)""")

        git_sub = git.Repo(".", search_parent_directories=True)

        tag_map = {i.commit: i.name for i in git_sub.tags}

        l_value = __hmtl_str_begin__

        current_value = str()
        for i in git_sub.iter_commits():
            if i in tag_map:
                l_value += "\n"
                l_value += "<h3> 版本 {} </h3>\n".format(tag_map[i])
            if self.is_null_mess(i.message):
                continue
            if current_value == i.message:
                continue
            else:
                current_value = i.message
                mess = i.message[:-1]
                mess = mess.replace("\n\n", "\n")
                mess = mess.replace("\n", "\n  ")
                if mess and mess[0] == ' ':
                    mess = mess[1::]
                if len(mess) > 50 or len(mess) == 0:
                    continue
                l_value += "<li> {}   (time: {})</li>\n".format(mess, i.committed_datetime.strftime("%y-%m-%d %I:%M"))

        l_value += __hmtl_str_end__
        with open(args[0], "w", encoding="utf-8") as file:
            file.write(l_value)
        # print(l_value)

    def is_null_mess(self, mess: str) -> bool:
        return re.search(self.mess_reg, mess) is not None


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("out_file",
                        nargs="?",
                        action="store",
                        help="必须参数, 输出的文件路径",
                        default=os.getcwd() + "/tmp.html")
    config = parser.parse_args(sys.argv[1:])
    git_log()(config.out_file)
