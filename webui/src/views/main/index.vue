<script lang="ts" setup>
import { reactive, ref, toRaw } from 'vue';

const iconList = [
  { classBase: 'bi bi-box-arrow-in-down', name: 'saveConfig' },
  { classBase: 'bi bi-box-arrow-in-up', name: 'readConfig' },
  { classBase: 'bi', name: 'connectServer', classClose: ' bi-play-fill', classActivate: ' bi-stop-fill' },
];

const inputList = [
  { classBase: 'bi bi-gear', name: 'ipConfig', title: 'IP配置：', value: 'ip' },
  { classBase: 'bi bi-gear', name: 'portConfig', title: '端口配置：', value: 'port' }
];

const origin: Record<string, any> = {
  ip: "192.168.4.1",
  port: "80",
  // 输入类型配置
  inputList: [
    { name: 1, status: -1 }, // status: -1禁用，0模拟量输入，1数字量输入
    { name: 2, status: -1 },
    { name: 3, status: -1 },
    { name: 4, status: -1 },
    { name: 5, status: -1 },
    { name: 6, status: -1 },
    { name: 7, status: -1 },
    { name: 8, status: -1 },
    { name: 9, status: -1 },
    { name: 10, status: -1 }
  ],
  // 输入范围配置
  nowInputList: [
    // [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}], // value:数字量值，max:模拟量上限，min：模拟量下限
  ],
  outputList: [
    { name: 1 }, // status: 0：数字量输出false，1：数字量输出true
    { name: 2 },
    { name: 3 },
    { name: 4 },
    { name: 5 },
    { name: 6 },
    { name: 7 },
    { name: 8 },
    { name: 9 },
    { name: 10 },
  ],
  // 输出值配置
  nowOutputList: [
    // [0, 0, 0, 0, 0, 0, 0, 0, 0, 0] // 数字量值
  ],
  configItemLength: 0 // 配置项长度
};

const config = reactive(origin); // 配置数据

const clickFunctionbase = (event: MouseEvent, name: string) =>
{
  const target = event.currentTarget as HTMLElement;

  // 创建 ripple 元素
  const ripple = document.createElement('span');
  ripple.className = 'ripple';

  const rect = target.getBoundingClientRect();
  const size = Math.max(rect.width, rect.height);
  ripple.style.width = ripple.style.height = `${size}px`;
  ripple.style.left = `${event.clientX - rect.left - size / 2}px`;
  ripple.style.top = `${event.clientY - rect.top - size / 2}px`;

  target.appendChild(ripple);

  // 动画结束后移除
  setTimeout(() =>
  {
    ripple.remove();
  }, 600);

  clickFunctionPack[name]();
};

let connectServerStatus = ref(false); // 连接状态

setInterval(async () =>
{
  if (connectServerStatus.value)
  {
    // 连接服务器
    console.log('连接服务器');

    const host = `http://${config.ip}:${config.port}/setting`;

    console.log(toRaw(config));

    fetch(host, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(toRaw(config)),
    })
      .then((response) => response.json())
      .then((data) =>
      {
        console.log('成功:', data);
      })
      .catch((error) =>
      {
        console.error('错误:', error);
      });
  }
}, 5000);

const clickFunctionPack: Record<string, () => void> = {
  saveConfig: () =>
  {
    const filename = prompt('请输入文件名（无需加 .json）：', 'config');
    if (!filename) return;

    const blob = new Blob([JSON.stringify(toRaw(config), null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${filename}.json`;
    a.click();
    URL.revokeObjectURL(url);
  },
  readConfig: () =>
  {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';
    input.onchange = async () =>
    {
      const file = input.files?.[0];
      if (file)
      {
        const text = await file.text();
        try
        {
          const json = JSON.parse(text);
          Object.assign(config, json);  // 覆盖原 config 内容（浅合并）
        } catch (err)
        {
          alert('文件格式不正确');
          console.error(err);
        }
      }
    };
    input.click();

  },
  connectServer: () =>
  {
    console.log('连接服务器');
    connectServerStatus.value = !connectServerStatus.value;
  },
};

const getInputWidth = () =>
{
  const max = config.inputList.length + config.outputList.length;
  const min = config.inputList.length + 2;
  const width = 98 * (min / max);
  return width;
};

const getOutputWidth = () =>
{
  const max = config.inputList.length + config.outputList.length;
  const min = config.outputList.length - 2;
  const width = 98 * (min / max);
  return width;
};

const addConfigList = () =>
{
  const newInputList = new Array(config.inputList.length).fill(null).map(() => ({}));
  config.nowInputList.push(newInputList);

  const newOutputList = new Array(config.outputList.length).map(() => 0);
  config.nowOutputList.push(newOutputList);

  config.configItemLength++;
};

const rmConfigList = () =>
{
  config.nowInputList.pop();
  config.nowOutputList.pop();

  config.configItemLength--;
  if (config.configItemLength < 0) { config.configItemLength = 0; }
};

const updateInputStatus = (event: Event, passage: number) =>
{
  const target = event.target as HTMLSelectElement;
  const status = Number(target.value);
  // item.status = status;
  config.nowInputList.forEach((element: any, index: number) =>
  {
    if (status == -1)
    {
      config.nowInputList[index][passage] = {};
    }

    // value:数字量值，max:模拟量上限，min：模拟量下限
    // status: -1禁用，0模拟量输入，1数字量输入
    if (status == 0)
    {
      delete config.nowInputList[index][passage].value;
    }

    if (status == 1)
    {
      delete config.nowInputList[index][passage].max;
      delete config.nowInputList[index][passage].min;
    }
  });

};
</script>

<template>
  <div class="mainView">
    <div class="headView">
      <!-- 读取配置 -->
      <div class="iconItemBoxs" v-for="item in iconList" @click="clickFunctionbase($event, item.name)">
        <div class="icon" :class="`${item.classBase} ${(connectServerStatus ? item.classActivate : item.classClose)}`">
        </div>
      </div>

      <div class="inputItemBox" v-for="item in inputList">
        <div class="title">{{ item.title }}</div>
        <input class="inputBox" v-model="config[item.value]"></input>
      </div>
      <!-- 目标IP -->
      <!-- 目标端口 -->
    </div>
    <div class="bodyViewBox">
      <div class="bodyView">
        <div class="splitBox"></div>
        <div class="mainViewBox">

          <!-- 状态栏 -->
          <div class="statusListBox">
            <!-- 行 -->
            <div class="rowItem"></div>
            <div class="rowItem" :style="{ writingMode: 'vertical-rl', textOrientation: 'upright' }">通道</div>
            <div class="rowItem" :style="{ writingMode: 'vertical-rl', textOrientation: 'upright' }">实时</div>

            <div class="rowItem" v-for="(item, index) of config.nowInputList">
              <!-- 列 -->
              {{ index + 1 }}
            </div>

            <div class="rowItem">
              <div class="addRowButtonBox bi bi-plus-circle" @click="addConfigList"></div>
              <div class="rmRowButtonBox bi bi-dash-circle" @click="rmConfigList"></div>
            </div>
          </div>

          <!-- 模拟量输入配置 -->
          <div class="inputViewListBox" :style="{ width: `${getInputWidth()}%` }">
            <!-- 行 -->
            <div class="rowItem">
              输入配置
            </div>

            <!-- 配置 -->
            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.inputList.length}%` }"
                v-for="(item, index) of config.inputList">
                <div class="itemTitle">{{ item.name }}</div>
                <div class="itemConfig">
                  <select @change="updateInputStatus($event, index)" v-model="item.status">
                    <option :value="-1">禁用</option>
                    <option :value="0">模拟量</option>
                    <option :value="1">数字量</option>
                  </select>
                </div>
              </div>
            </div>

            <!-- 实时值 -->
            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.inputList.length}%` }"
                v-for="item of config.inputList"></div>
            </div>

            <!-- 正式配置 -->
            <div class="rowItem" v-for="rowItem of config.nowInputList">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / rowItem.length}%` }"
                v-for="(columnItem, columnIndex) of rowItem">

                <div class="analogConfigBox" v-if="config.inputList[columnIndex].status == 0">
                  <div class="analogConfigItem">
                    <div class="analogIcon bi bi-arrow-up-short"></div>
                    <input type="text" v-model="columnItem.max"></input>
                  </div>

                  <div class="analogConfigItem">
                    <div class="analogIcon bi bi-arrow-down-short"></div>
                    <input type="text" v-model="columnItem.min"></input>
                  </div>
                </div>

                <input type="checkbox" v-if="config.inputList[columnIndex].status == 1" class="checkbox"
                  v-model="columnItem.value" :true-value="1" :false-value="0"></input>
              </div>
            </div>
          </div>

          <!-- 数字量输出配置 -->
          <div class="outputViewListBox" :style="{ width: `${getOutputWidth()}%` }">
            <div class="rowItem">
              输出配置
            </div>

            <!-- 配置 -->
            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.outputList.length}%` }"
                v-for="item of config.inputList">{{ item.name }}</div>
            </div>

            <!-- 实时值 -->
            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.outputList.length}%` }"
                v-for="item of config.inputList">

              </div>
            </div>

            <!-- 正式配置 -->
            <div class="rowItem" v-for="rowItem of config.nowOutputList">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / rowItem.length}%` }" v-for="(columnItem, columnIndex) of rowItem">
                <input type="checkbox" class="checkbox" v-model="rowItem[columnIndex]" :true-value="1" :false-value="0"></input>
              </div>
            </div>

          </div>
        </div>
        <div class="splitBox"></div>
      </div>
    </div>
  </div>
</template>

<style scoped lang="scss">
.mainView {
  --mainFontColor: #8E8E8E;

  .headView {
    height: 5%;
    width: 100%;
    background-color: #323233;
    display: flex;
    align-items: center;
    color: var(--mainFontColor);

    .iconItemBoxs {
      position: relative; // 关键
      overflow: hidden; // 防止波纹超出
      margin-left: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      user-select: none;
      background-color: #252526;

      .icon {
        width: 25px;
        height: 25px;
        border: 1px solid var(--mainFontColor);
        border-radius: 5px;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size: 20px;
        z-index: 2;
      }

      .ripple {
        position: absolute;
        border-radius: 50%;
        background-color: rgba(255, 255, 255, 0.5);
        transform: scale(0);
        animation: ripple 0.6s linear;
        pointer-events: none;
      }

      @keyframes ripple {
        to {
          transform: scale(4);
          opacity: 0;
        }
      }
    }

    .inputItemBox {
      margin-left: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      user-select: none;

      .title {
        font-size: 20px;
      }

      .inputBox {
        border-radius: 5px;
        border: 1px solid var(--mainFontColor);
        background-color: #787878;
        font-size: 20px;
      }
    }
  }

  .bodyViewBox {
    height: 95%;
    width: 100%;
    background-color: #1E1E1E;
    display: flex;
    align-items: center;
    justify-content: center;

    .bodyView {
      height: 100%;
      width: 95%;
      overflow-y: auto;
      scrollbar-width: none;
      -ms-overflow-style: none;

      .splitBox {
        height: 5%;
        width: 100%;
      }

      .splitBox::-webkit-scrollbar {
        display: none;
        /* Chrome、Safari 隐藏滚动条 */
      }

      .mainViewBox {
        height: 90%;
        width: 100%;
        background-color: #323233;
        display: flex;
        align-items: center;


        .statusListBox {
          width: 2%;
          height: 100%;
          border-right: 1px solid #4a4a4a;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            color: var(--mainFontColor);

            .addRowButtonBox {
              width: 100%;
              height: 50%;
              display: flex;
              align-items: center;
              justify-content: center;
              cursor: pointer;
              font-size: 20px;
            }

            .rmRowButtonBox {
              width: 100%;
              height: 50%;
              display: flex;
              align-items: center;
              justify-content: center;
              cursor: pointer;
              font-size: 20px;
            }
          }
        }

        .inputViewListBox {
          height: 100%;
          border-right: 1px solid #4a4a4a;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            flex-direction: row;
            justify-content: center;
            color: var(--mainFontColor);

            .columnItem {
              height: 100%;
              border-right: 1px solid #4a4a4a;
              display: flex;
              align-items: center;
              justify-content: center;

              .itemTitle {
                height: 100%;
                width: 30%;
                display: flex;
                align-items: center;
                justify-content: center;
              }

              .itemConfig {
                height: 100%;
                width: 70%;
                display: flex;
                align-items: center;
                justify-content: center;
              }

              .analogConfigBox {
                height: 100%;
                width: 100%;
                display: flex;
                flex-direction: column;
                align-items: center;
                justify-content: space-around;

                .analogConfigItem {
                  width: 90%;
                  height: 40%;
                  display: flex;
                  align-items: center;
                  justify-content: space-around;

                  .analogIcon {
                    width: 30%;
                    height: 70%;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    font-size: 2rem;

                  }

                  input {
                    width: 60%;
                    height: 70%;
                    border-radius: 10px;
                    text-align: center;
                  }
                }
              }

              .checkbox {
                height: 30px;
                width: 30px;
              }
            }
          }
        }

        .outputViewListBox {
          height: 100%;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            flex-direction: row;
            justify-content: center;
            color: var(--mainFontColor);

            .columnItem {
              height: 100%;
              border-right: 1px solid #4a4a4a;
              display: flex;
              align-items: center;
              justify-content: center;

              .checkbox {
                height: 30px;
                width: 30px;
              }
            }
          }
        }
      }
    }
  }
}
</style>

<style lang="scss">
.mainView .headView .iconItemBoxs {

  .ripple {
    position: absolute;
    border-radius: 50%;
    background-color: rgba(255, 255, 255, 0.5);
    transform: scale(0);
    animation: ripple 0.6s linear;
    pointer-events: none;
  }

  @keyframes ripple {
    to {
      transform: scale(4);
      opacity: 0;
    }
  }
}
</style>