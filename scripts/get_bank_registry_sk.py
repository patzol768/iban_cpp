import json

import pandas

URL = "https://www.nbs.sk/_img/Documents/_PlatobneSystemy/EUROSIPS/Directory_IC_DPS_SR.csv"


def process():
    datas = pandas.read_csv(URL, encoding="cp1250", delimiter=";")
    datas = datas.dropna(how="all")
    datas.fillna("", inplace=True)

    registry = []

    for row in datas.itertuples(index=False):
        bank_code, name, bic = row[:3]

        bic = str(bic).strip().upper()

        registry.append(
            {
                "country_code": "SK",
                "primary": True,
                "bic": bic,
                "bank_code": str(bank_code).strip().zfill(4),
                "name": str(name).strip(),
                "short_name": str(name).strip(),
            }
        )

    print(f"Fetched {len(registry)} bank records")
    return registry


if __name__ == "__main__":
    with open("registry/bank_registry/generated_sk.json", "w") as fp:
        json.dump(process(), fp, indent=2)
