from collections import defaultdict
from opencc import OpenCC

converter = OpenCC()

tc_cat = defaultdict(set)

with open("./resource/zh_gsd-ud-train.conllu") as file:
    for line in file.readlines():
        if line[0] == "#":
            continue  # skip comments
        tags = line.split("\t")
        if len(tags) < 5:
            continue  # skip invalid lines
        [form, lemma, upos, xpos] = tags[1:5]
        # print(form, upos)
        tc_cat[form].add(upos)

# print(tc_cat)
# print(len(tc_cat))

tc_sc = dict()

with open("./resource/cedict_ts.u8") as file:
    for line in file.readlines():
        tags = line.split(" ")
        if len(tags) < 2:
            continue  # skip invalid lines
        [sc, tc] = tags[0:2]

        tc_sc[tc] = sc

tts_keys = tc_sc.keys()
with open("./resource/cat_dict.txt", "w") as file:
    for tc in tc_cat.keys():
        if tc not in tts_keys:
            sc = converter.convert(tc)
            print(
                f"cannot find word {tc} with a simplified form, auto converted to {sc}"
            )
        else:
            sc = tc_sc[tc]

        cats = tc_cat[tc]
        for cat in cats:
            print(f"{sc} {tc} {cat}", file=file)
