DELETE FROM `gossip_menu_option` WHERE (`menu_id`=13830 AND `id`=0) OR (`menu_id`=9821 AND `id`=1) OR (`menu_id`=9821 AND `id`=3) OR (`menu_id`=14636 AND `id`=0) OR (`menu_id`=13733 AND `id`=0);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `box_coded`, `box_money`, `box_text`) VALUES
(13830, 0, 2, 'Мне бы хотелось кое-куда слетать.', 0, 0, ''), -- 61512
(9821, 1, 1, 'Я ищу пропавшего спутника.', 0, 0, ''), -- 66246
(9821, 3, 0, 'Мне бы хотелось воскресить и исцелить моих боевых питомцев.', 0, 0, ''), -- 66246
(14636, 0, 1, 'Позвольте мне взглянуть на ваш товар.', 0, 0, ''), -- 65171
(13733, 0, 0, 'Осмотреть тело.', 0, 0, ''); -- 60899
