DELETE FROM `gossip_menu_option` WHERE (`menu_id`=14640 AND `id`=0) OR (`menu_id`=13847 AND `id`=0) OR (`menu_id`=14663 AND `id`=0);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `box_coded`, `box_money`, `box_text`) VALUES
(14640, 0, 1, 'Мне бы хотелось купить что-нибудь у вас.', 0, 0, ''), -- 65172
(13847, 0, 0, 'Мне нужно в Клакси''весс.', 0, 0, ''), -- 62202
(14663, 0, 1, 'Мне бы хотелось купить что-нибудь у вас.', 0, 0, ''); -- 64599
